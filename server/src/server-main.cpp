#include <iostream>
#include "VoxServer.h"
#include "DataModel.h"

#include "vox/diag/logging.h"

std::shared_ptr<VoxServer> server;
std::shared_ptr<IDataModel> dbLink;

void handleMsg(proto::vox::ServerIncomingMessage const& msg,
    boost::uuids::uuid const& sessionUid)
{
    std::cout << "MessageReceived:<";

    switch (msg.msg_case())
    {
        case proto::vox::ServerIncomingMessage::kAuthRequest:
        {
            auto const& request = msg.auth_request();
            VOX_LOG_INFO("AuthRequest for {}", request.login());

            proto::vox::ServerOutgoingMessage oMsg;
            auto& resultMsg = *oMsg.mutable_auth_result();

            const auto user = dbLink->getUser(request.login());

            if (user)
            {
                VOX_LOG_INFO("User {} found ", request.login());
                resultMsg.set_session_uuid(uuidToBytes(generateUuid()));
                resultMsg.set_user_uuid(uuidToBytes(user->uuid));
            }
            else
            {
                VOX_LOG_INFO("User {} NOT found ", request.login());
            }

            server->sendMessage(oMsg, sessionUid);
            break;
        }
        case proto::vox::ServerIncomingMessage::kRegRequest:
        {
            std::cout << "kRegRequest";
            break;
        }
        case proto::vox::ServerIncomingMessage::kHeartbeat:
        {
            std::cout << "kHeartbeat";
            break;
        }
        case proto::vox::ServerIncomingMessage::kChatListRequest:
        {
            auto const& request = msg.chat_list_request();
            const auto uuid = uuidFromBytes(request.user_uuid());

            VOX_LOG_INFO("Chat list request for {}", uuidToString(uuid);



          break;
        }
        default:
            break;
    }

    std::cout << ">" << std::endl;

    //proto::vox::ServerOutgoingMessage oMsg;
    //oMsg.set_type_string("Accepted:" + msg.type_string());

    //server->sendMessage(oMsg, sessionUid);
}

int main()
{ 
    std::cout << "HELLOWORLD" << std::endl;

    server = std::make_shared<VoxServer>(handleMsg);
    dbLink = std::make_shared<DataModel>("postgresql://postgres:postgres@localhost:5432/postgres");

    server->startDetached(5543);

    int i = 0;
    std::cin >> i;
    return 0;
}