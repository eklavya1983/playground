#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <actor/RemoteActor.h>
#include <actor/ActorServer.h>

namespace actor {

using namespace apache::thrift;
using namespace apache::thrift::async;
using namespace apache::thrift::transport;

RemoteActor::RemoteActor(ActorSystem *system,
                         folly::EventBase *eventBase,
                         const ActorInfo &info)
: NotificationQueueActor(system, eventBase)
{
    // TODO: Check if info is necessary
    info_ = info;    
    // TODO: Move this into init
    // TODO: Handle socket errors
    std::shared_ptr<TAsyncSocket> socket(
        TAsyncSocket::newSocket(eventBase, info.ip, info.port));

    auto client_channel = HeaderClientChannel::newChannel(socket);
    client_.reset(new ServiceApiAsyncClient(std::move(client_channel)));
}

RemoteActor::~RemoteActor() {
}

void RemoteActor::initBehaviors_() {
    functionalBehavior_ += {
        on(Other) >> [this]() {
            /* serialize */
            std::unique_ptr<folly::IOBuf> buf;
            auto &msgSerializerF = gMsgMapTbl->at(typeId()).first;
            msgSerializerF(*msg(), buf);
            /* send the message */
            client_->actorMessage([](ClientReceiveState&& state) {}, msg()->header(), *buf);

        }
    };
}

}  // namespace actor
