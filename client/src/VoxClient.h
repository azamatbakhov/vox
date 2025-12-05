#pragma once

#include "vox/async/service.h"
#include "vox/rtti/rtti_object.h"
#include "vox/rtti/rtti_impl.h"
#include "vox/error/error.h"

#include "vox/proto/ServerIncomingMessage.pb.h"
#include "vox/proto/ServerOutgoingMessage.pb.h"

#include <boost/asio.hpp>

#include <cstdio>
#include <atomic>
#include <thread>


class VoxClient final : virtual vox::IRttiObject
{
    VOX_CLASS(VoxClient)

    enum class Operation
    {
        Connection,
        MessageSend,
        MessageRecieved,
    };

    struct OperationCompletionManifold
    {
        Operation operation{};

        std::optional<proto::vox::ServerOutgoingMessage> serverOutgoingMsg;
        std::optional<proto::vox::ServerIncomingMessage> serverIncomingMsg;

        vox::Error error;
    };

    using OperationCompletionHandler = std::function<void(OperationCompletionManifold const&)>;

    explicit VoxClient(OperationCompletionHandler operationHandler,
        std::string const& host, uint16_t port);

    ~VoxClient();

    void start();

    void sendMessage(proto::vox::ServerIncomingMessage const& msg);

private:
    void resolve();
    void connect(boost::asio::ip::tcp::resolver::results_type endpoints);
    void scheduleKeepAliveHeartbeat();
    void stopKeepAliveHeartbeat();
    void sendKeepAliveHeartbeat();

    void fireupOperationTimer(std::function<void()> callback = {});
    void stopTimeoutAwaiting();

    void scheduleReconnect();
    void handleIncomingMessages();

    void reconnect();

    void sendMessageImpl(proto::vox::ServerIncomingMessage const& msg, bool reportUser = false);

    OperationCompletionHandler m_operationHandler;
    std::string m_host;
    uint16_t m_port{};

    boost::asio::streambuf m_streambuf;

    std::thread m_thread;

    boost::asio::io_context m_ioContext;
    boost::asio::ip::tcp::resolver m_resolver;
    boost::asio::ip::tcp::socket m_socket;

    boost::asio::steady_timer m_timer;
    boost::asio::steady_timer m_reconnectTimer;
    boost::asio::steady_timer m_keepAliveTimer;

    std::list<std::vector<char>> m_pendingSendBuffers;

    std::chrono::seconds m_operationTimeout{10};
    std::chrono::seconds m_keepAliveTimeout{15};
    std::chrono::seconds m_reconnectTimeout{5};
};
