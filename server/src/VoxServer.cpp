#include "VoxServer.h"
#include "vox/utils/scope_guard.h"

#include <format>


VoxServer::VoxServer(MessageHandler handler)
    : m_messageHandler(std::move(handler))
    , m_ioContext(1)
{
}

boost::asio::awaitable<void> VoxServer::start(uint16_t serverBindPort)
{
    try
    {
        auto executor = co_await boost::asio::this_coro::executor;
        
        m_acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(
            executor, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), serverBindPort});

        while (!m_stopped.test(std::memory_order_acquire))
        {
            auto socket = co_await m_acceptor->async_accept(boost::asio::use_awaitable);
            std::cout << "Accepted new client" << std::endl;

            boost::asio::co_spawn(executor, process(std::move(socket)), boost::asio::detached);
        }

    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        throw std::runtime_error(std::format("Unable to create <{}>({})", serverBindPort, e.what()));
    }
}

boost::asio::awaitable<void> 
VoxServer::sendImpl(proto::vox::ServerOutgoingMessage const& msg,
    boost::asio::ip::tcp::socket& socket)
{
    std::vector<char> data(msg.ByteSizeLong() + 3);

    msg.SerializePartialToArray(data.data(), msg.ByteSizeLong());
    data[data.size() - 1] = '@';
    data[data.size() - 2] = '@';
    data[data.size() - 3] = '@';

    co_await boost::asio::async_write(socket,
        boost::asio::buffer(data), boost::asio::use_awaitable);
}

VoxServer::~VoxServer()
{
    try
    {
        m_stopped.test_and_set(std::memory_order_release);
        m_ioContext.stop();
        m_thread.join();
    }
    catch (...)
    {

    }
}

void VoxServer::startDetached(uint16_t serverBindPort)
{
    boost::asio::co_spawn(m_ioContext, start(serverBindPort), boost::asio::detached);

    m_thread = std::thread(&boost::asio::io_context::run, &m_ioContext);
}

bool VoxServer::sendMessage(proto::vox::ServerOutgoingMessage const& msg, boost::uuids::uuid const& session)
{   
    try
    {
        auto itSession = m_session.find(session);
        if (itSession == m_session.end())
        {
            std::cout << "Can't Send a message" << std::endl;
            return false;
        }

        boost::asio::co_spawn(m_ioContext, 
            sendImpl(msg, *itSession->second.socket), boost::asio::detached);        

    }
    catch (std::exception const& e)
    {
       std::cout << "ZmqBasicServer: failed to send a message:" << e.what();
    }

    return true;
}

boost::asio::awaitable<void> VoxServer::process(boost::asio::ip::tcp::socket socket)
{
    try
    {
        char data[10 * 1024];
        const auto sessionUuid = generateUuid();

        m_session.insert({sessionUuid, SessionData{.socket = &socket}});

        while (!m_stopped.test(std::memory_order_acquire))
        {
            const std::size_t readSize = co_await socket
                .async_read_some(boost::asio::buffer(data), boost::asio::use_awaitable);
            
            proto::vox::ServerIncomingMessage msg;
            if (msg.ParseFromArray(data, readSize))
            {
                std::invoke(m_messageHandler, msg, sessionUuid);
            }
            else
            {
                std::cout << "Server read : failed to parse" << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::printf("echo Exception: %s\n", e.what());
    }
}
