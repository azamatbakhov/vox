#include "VoxClient.h"
#include "vox/diag/logging.h"
#include "vox/utils/uuid.h"
#include "vox/utils/enum.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

std::shared_ptr<VoxClient> client;
std::string clientId{"unknown"};

void handle(const VoxClient::OperationCompletionManifold& operation)
{

    if (operation.operation == VoxClient::Operation::Connection && !operation.error)
    {
        VOX_LOG_DEBUG("CONNECTED:{}", bool(operation.error));
        if (!operation.error)
        {
            proto::vox::ServerIncomingMessage incMsg;
            proto::vox::AuthorizationRequest& request = *incMsg.mutable_auth_request();
            request.set_login(clientId);

            client->sendMessage(incMsg);
        }
    }
    else if (operation.operation == VoxClient::Operation::MessageSend)
    {
    }
    else if (operation.operation == VoxClient::Operation::MessageRecieved)
    {
        const auto& msg = *operation.serverOutgoingMsg;

        switch (msg.msg_case())
        {
            case proto::vox::ServerOutgoingMessage::kAuthResult:
            {
                const auto& authResult = msg.auth_result();

                if (authResult.user_uuid().empty())
                {
                    VOX_LOG_INFO("Auth DICLINED");
                }
                else
                {
                    VOX_LOG_INFO("Auth ACCEPTED: SESSION={}, user={}",
                                 uuidToString(uuidFromBytes(authResult.session_uuid())),
                                 uuidToString(uuidFromBytes(authResult.user_uuid())));
                }
                break;
            }
            case proto::vox::ServerOutgoingMessage::kRegResult:
            {
                break;
            }
            default:
                VOX_LOG_WARNING("UNKNOWN Message type:{}", +operation.operation);
                break;
        }
    }
}

int main(int argc, char* argv[])
{ 
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject *obj, const QUrl &objUrl) {
          if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();

    //if (argc > 1)
    //{
    //    clientId = argv[1];
    //}

    //VOX_LOG_INFO("Client {}. Enter <q> to exit", clientId);

    //client = std::make_shared<VoxClient>(handle, "localhost", 5543);
    //
    //client->start();

    //do
    //{
    //    std::string command;
    //    std::cin >> command;

    //    if (command == "q")
    //        break;

    //} while (true);

    
    return 0;
}