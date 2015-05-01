#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <util/Log.h>
#include <actor/ActorUtil.h>
#include <actor/RemoteActor.h>
#include <actor/ActorServer.h>

namespace actor {

using namespace apache::thrift;
using namespace apache::thrift::async;
using namespace apache::thrift::transport;

struct RemoteActorRequestCb : apache::thrift::RequestCallback {
    virtual void requestSent() {}
    virtual void replyReceived(ClientReceiveState&&) {}
    virtual void requestError(ClientReceiveState&& state) {
        LOG(ERROR) << state.exceptionWrapper().what();
    }
};

RemoteActor::RemoteActor(ActorSystem *system,
                         folly::EventBase *eventBase)
: NotificationQueueActor(system, eventBase)
{
}

RemoteActor::~RemoteActor() {
}

void RemoteActor::initBehaviors_() {
    NotificationQueueActor::initBehaviors_();
    initBehavior_ += {
        on(Init) >> [this]() {
            ALog(INFO) << "Initng remote actor";
        },
        on(UpdateActorInfo) >> [this]() {
            info_ = payload<UpdateActorInfo>().info;

            /* Create new socket */
            std::shared_ptr<TAsyncSocket> socket(
                TAsyncSocket::newSocket(eventBase_, info_.ip, info_.port));
            auto client_channel = HeaderClientChannel::newChannel(socket);
            client_.reset(new ServiceApiAsyncClient(std::move(client_channel)));
            // TODO: Error handling

            changeBehavior(&functionalBehavior_);
        }
    };
    functionalBehavior_ += {
        on(Other) >> [this]() {
            /* serialize */
            std::unique_ptr<folly::IOBuf> buf;
            auto &msgSerializerF = gMsgMapTbl->at(typeId()).first;
            msgSerializerF(*msg(), buf);
            /* send the message */
            client_->actorMessage(
                std::unique_ptr<apache::thrift::RequestCallback>(new RemoteActorRequestCb()),
                msg()->header(),
                *buf);
        }
    };
}

}  // namespace actor
