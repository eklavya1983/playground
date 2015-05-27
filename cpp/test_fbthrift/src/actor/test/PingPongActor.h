#include <actor/Actor.hpp>
#include <actor/ActorRequest.h>
#include <actor/ActorSystem.h>

namespace actor {
namespace test {

struct Ping : LocalPayload<> {};
struct Pong : LocalPayload<> {};
struct SendPing : LocalPayload<> {
    ActorId to;
};
struct SendPingReq : LocalPayload<> {
    ActorId to;
};

/**
* @brief Helper actor for testing
*/
struct PingPongActor : NotificationQueueActor {
    using NotificationQueueActor::NotificationQueueActor;

    static const char* className() {return "PingPongActor";} 

    int32_t         pingCnt = 0;
    int32_t         pongCnt = 0;
    int32_t         trackedPongCnt = 0;
 protected:
    virtual void initBehaviors_() override {
        NotificationQueueActor::initBehaviors_();
        initBehavior_ += {
            on(Init) >> [this]() {
                changeBehavior(&functionalBehavior_);
            }
        };
        functionalBehavior_ += {
            on(Ping) >> [this]() {
                pingCnt++;
                REPLY(Pong, std::make_shared<Pong>());
            },
            onresp(Pong) >> [this]() {
                if (curMsg_->isTracked()) {
                    tracker_->handleResponse(std::move(*curMsg_));
                } else {
                    pongCnt++;
                }
            },
            on(SendPing) >> [this]() {
                ROUTE(makeActorMsg<Ping>(myId(), payload<SendPing>().to,
                                         std::make_shared<Ping>()));
            },
            on(SendPingReq) >> [this]() {
                auto req = tracker_->allocRequest<ActorRequest>();
                req->from(myId_)
                    .toActor(payload<SendPingReq>().to)
                    .withPayload(std::make_shared<Ping>())
                    .fire()
                    .then([req, this]() {
                        trackedPongCnt++;
                    });
            }
        };
    }
};

}  // namespace test
}  // namespace actor
