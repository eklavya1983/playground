#include <actor/Actor.h>
#include <actor/ActorRequest.h>

namespace actor {
namespace test {

struct Ping {};
struct Pong {};
struct SendPing {
    ActorId to;
};
struct SendQuorumPing {
    std::vector<ActorId> toActors;
};

/**
* @brief Helper actor for testing
*/
struct PingPongActor : NotificationQueueActor {
    int32_t         pingCnt = 0;
    int32_t         pongCnt = 0;
 protected:
    virtual void initBehaviors_() override {
        functionalBehavior_ += {
            on(Ping) >> [this]() {
                pingCnt++;
                REPLY(Pong, std::make_shared<Pong>());
            },
            onresp(Pong) >> [this]() {
                if (curMsg_->isTracked()) {
                    tracker_->onResponse(std::move(*curMsg_));
                } else {
                    pongCnt++;
                }
            },
            on(SendPing) >> [this]() {
                SEND(makeActorMsg<Ping>(myId(), payload<SendPing>().to,
                                        std::move(payload)));
            },
            on(SendQuorumPing) >> [this]() {
                auto& req = tracker_->allocRequest<QuorumRequest>();
                req.toActors(payload<SendQuorumPing>().toActors)
                    .withQuorum(payload<SendQuorumPing>().toActors.size())
                    .fire();
            }
        };
    }
};

}  // namespace test
}  // namespace actor
