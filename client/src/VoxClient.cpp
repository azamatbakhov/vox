#include "VoxClient.h"
#include "vox/utils/scope_guard.h"
#include "vox/diag/logging.h"
#include "vox/kernel/message.h"

#include <boost/asio.hpp>

#include <format>

namespace
{
    std::atomic_flag stopped = ATOMIC_FLAG_INIT;
}

VoxClient::VoxClient(OperationCompletionHandler operationHandler,
    std::string const& host, uint16_t port)
    : m_operationHandler(std::move(operationHandler))
    , m_host(host)
    , m_port(port)
    , m_ioContext(1)
    , m_resolver(m_ioContext)
    , m_socket(m_ioContext)
    , m_timer(m_ioContext)
    , m_reconnectTimer(m_ioContext)
    , m_keepAliveTimer(m_ioContext)
{
}

VoxClient::~VoxClient()
{
    try
    {
        stopped.test_and_set(std::memory_order_release);
        m_ioContext.stop();
        m_thread.join();
    }
    catch (...)
    {

    }
}

void VoxClient::start()
{
    resolve();

    m_thread = std::thread(&boost::asio::io_context::run, &m_ioContext);
}

void VoxClient::sendMessage(proto::vox::ServerIncomingMessage const& msg)
{
    sendMessageImpl(msg, true);
}

void VoxClient::fireupOperationTimer(std::function<void()> callback)
{
    stopTimeoutAwaiting();

    m_timer.expires_after(m_operationTimeout);
    m_timer.async_wait([this, callback](const boost::system::error_code& ec)
    {
        if (ec || stopped.test(std::memory_order_acquire))
        {
            return;
        }

        VOX_LOG_WARNING("Network operation timeout");
        if (callback)
        {
            callback();
        }

        std::invoke(m_operationHandler, OperationCompletionManifold{
                                            .operation = Operation::Connection,
                                            .error = vox::Error::fromBoostErrorCode(ec)
        });

        scheduleReconnect();
    });
}

void VoxClient::stopTimeoutAwaiting()
{
    try
    {
        m_timer.cancel();
    }
    catch (...)
    {   
    }
}

void VoxClient::scheduleReconnect()
{
    VOX_LOG_INFO("Schedule reconnect after {}", m_reconnectTimeout);

    stopKeepAliveHeartbeat();
    stopTimeoutAwaiting();

    m_reconnectTimer.expires_after(m_reconnectTimeout);
    m_reconnectTimer.async_wait([this](const boost::system::error_code& error)
    {
        if (error == boost::asio::error::operation_aborted)
        {
            return;
        }

        if (stopped.test(std::memory_order_acquire))
        {
            return;
        }

        reconnect();
    });
}

void VoxClient::handleIncomingMessages()
{
    if (stopped.test(std::memory_order_acquire))
    {
        return;
    }

    boost::system::error_code ec;
    if (m_socket.available(ec) > 0)
    {
        fireupOperationTimer();
    }

    boost::asio::async_read_until(m_socket, m_streambuf, vox::pkgDelimiter(),
        [this](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        stopTimeoutAwaiting();
        if (error == boost::asio::error::operation_aborted)
        {
            return;
        }
        
        std::vector<char> target_vector(m_streambuf.size());
        boost::asio::buffer_copy(boost::asio::buffer(target_vector), m_streambuf.data());

        proto::vox::ServerOutgoingMessage msg;
        if (bytes_transferred > vox::pkgDelimiterSize() && 
            msg.ParseFromArray(m_streambuf.data().data(), bytes_transferred - vox::pkgDelimiterSize()))
        {
            std::invoke(m_operationHandler, OperationCompletionManifold{
                .operation = Operation::MessageRecieved,
                .serverOutgoingMsg = msg
            });
        }

        m_streambuf.consume(bytes_transferred);
        handleIncomingMessages();
    });

}

void VoxClient::reconnect()
{
    boost::system::error_code ec;
    m_socket.close(ec);

    resolve();
}

void VoxClient::resolve()
{
    fireupOperationTimer();

    m_resolver.async_resolve(m_host, std::to_string(m_port), [this](
        const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::results_type results)
    {
        stopTimeoutAwaiting();
        if (ec == boost::asio::error::operation_aborted)
        {
            return;
        }

        VOX_LOG_INFO("Resolved success {} endpoint(s). Trying to connect", results.size());
        connect(results);
    });

}

void VoxClient::connect(boost::asio::ip::tcp::resolver::results_type endpoints)
{
    fireupOperationTimer();

    boost::asio::async_connect(m_socket, endpoints, [this](const boost::system::error_code& ec,
        boost::asio::ip::tcp::endpoint endpoint)
    {
        stopTimeoutAwaiting();

        if (ec)
        {
            if (ec == boost::asio::error::operation_aborted)
            {
                return;
            }

            VOX_LOG_ERROR("Connection failure with: {} ", ec.what());

            m_operationHandler({.operation = Operation::Connection,
                                .error = vox::Error::fromBoostErrorCode(ec)
            });

            scheduleReconnect();
            return;
        }

        VOX_LOG_INFO("Connected to {}:{}", endpoint.address().to_string(), endpoint.port());

        m_operationHandler({ .operation = Operation::Connection });
        scheduleKeepAliveHeartbeat();
        handleIncomingMessages();

        m_socket.local_endpoint().address().to_string();
    });
}

void VoxClient::scheduleKeepAliveHeartbeat()
{
    m_keepAliveTimer.expires_after(m_keepAliveTimeout);
    m_keepAliveTimer.async_wait([this](const boost::system::error_code& ec)
    {
        if (ec)
        {
            if (ec == boost::asio::error::operation_aborted)
            {
                return;
            }

            VOX_LOG_ERROR("Heartbeat timer error. Reschedule...", ec.what());

            scheduleKeepAliveHeartbeat();
            return;
        }

        sendKeepAliveHeartbeat();
    });
}

void VoxClient::stopKeepAliveHeartbeat()
{
    try
    {
        m_keepAliveTimer.cancel();
    }
    catch (...)
    {
        // TODO
    }
}

void VoxClient::sendKeepAliveHeartbeat()
{
    proto::vox::ServerIncomingMessage msg;
    *msg.mutable_heartbeat() = proto::vox::Heartbeat();

    sendMessageImpl(msg, false);
}

void VoxClient::sendMessageImpl(proto::vox::ServerIncomingMessage const& msg, bool reportUser)
{
    m_pendingSendBuffers.emplace_back(std::vector<char>(msg.ByteSizeLong()));
    auto it = std::prev(m_pendingSendBuffers.end());

    auto& data = m_pendingSendBuffers.back();

    msg.SerializePartialToArray(data.data(), data.size());

    fireupOperationTimer([this, it]
    {
        m_pendingSendBuffers.erase(it);
    });

    stopKeepAliveHeartbeat();

    boost::asio::async_write(m_socket, boost::asio::buffer(data), [this, msg, it, reportUser]
    (const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        stopTimeoutAwaiting();

        if (stopped.test(std::memory_order_acquire))
        {
            return;
        }

        scheduleKeepAliveHeartbeat();

        m_pendingSendBuffers.erase(it);
        if (reportUser)
        {
            std::invoke(m_operationHandler, OperationCompletionManifold{
                                                .operation = Operation::MessageSend,
                                                .serverIncomingMsg = msg,
                                                .error = vox::Error::fromBoostErrorCode(error)});
        }
    });
}
