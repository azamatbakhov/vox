#pragma once

#include "vox/async/service.h"
#include "vox/rtti/rtti_object.h"
#include "vox/rtti/rtti_impl.h"

#include "vox/proto/ServerIncomingMessage.pb.h"
#include "vox/proto/ServerOutgoingMessage.pb.h"
#include <vox/utils/uuid.h>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/write.hpp>


#include <cstdio>


#include <atomic>
#include <thread>


class VoxServer final : virtual vox::IRttiObject
{
    VOX_CLASS(VoxServer)

    /**
     * 
     */
    using MessageHandler = std::function<void(proto::vox::ServerIncomingMessage const&,
                                              boost::uuids::uuid const&)>;

    explicit VoxServer(MessageHandler handler);


    ~VoxServer();
    void startDetached(uint16_t serverBindPort);

    bool sendMessage(proto::vox::ServerOutgoingMessage const& msg,
        boost::uuids::uuid const& session);

private:
    boost::asio::awaitable<void> start(uint16_t serverBindPort);
    boost::asio::awaitable<void> sendImpl(proto::vox::ServerOutgoingMessage const& msg,
                                          boost::asio::ip::tcp::socket& socket);

    //boost::asio::awaitable<void> listener();
    boost::asio::awaitable<void> process(boost::asio::ip::tcp::socket socket);

    MessageHandler m_messageHandler;

    std::atomic_flag m_stopped = ATOMIC_FLAG_INIT;

    boost::asio::io_context m_ioContext;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;

    std::thread m_thread;

    struct SessionData
    {
        boost::asio::ip::tcp::socket* socket = nullptr;
    };

    std::unordered_map<boost::uuids::uuid, SessionData> m_session;
};
