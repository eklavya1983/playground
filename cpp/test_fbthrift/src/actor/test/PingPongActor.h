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
struct SendQuorumPing : LocalPayload<> {
    std::vector<ActorId> toActors;
};

/**
* @brief Helper actor for testing
*/
struct PingPongActor : NotificationQueueActor {
    using NotificationQueueActor::NotificationQueueActor;

    static const char* className() {return "PingPongActor";} 

    int32_t         pingCnt = 0;
    int32_t         pongCnt = 0;
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
            on(SendQuorumPing) >> [this]() {
                auto& req = tracker_->allocRequest<QuorumRequest>();
                auto completeCb = [this](const Error &e, QuorumRequest &q) {
                    if (e == Error::ERR_OK) {
                        pongCnt += q.ackSuccessCnt();
                    }
                };
                req.toActors(payload<SendQuorumPing>().toActors)
                    .withQuorum(payload<SendQuorumPing>().toActors.size())
                    .withCompletionCb(completeCb)
                    .fire();
            }
        };
    }
};

}  // namespace test
}  // namespace actor
