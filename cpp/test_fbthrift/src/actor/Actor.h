#pragma once

#include <string>
#include <folly/io/async/NotificationQueue.h>
#include <actor/ActorMsg.h>
#include <actor/Behavior.h>

#if 0
#define CASE(id) case static_cast<int>((id))
#define SWITCH(msg) switch (actorMsgType(msg))
#define ACTORMSGHANDLER(behavior, type, handler) \
    behavior[static_cast<int>((type))] = std::bind(&handler, this, std::placeholders::_1)
#endif

namespace actor {
using namespace cpp2;

using ActorHandlerF = std::function<void (ActorMsg &&)>;
// using Behavior = std::unordered_map<ActorMsgType, ActorHandlerF>;

struct Actor {
    Actor();

    void setId(const ActorId &id);
    ActorId getId() const;

    virtual void changeBhavior(Behavior *behavior);

    virtual void send(ActorMsg &&msg) = 0;
    virtual void handle(ActorMsg &&msg);

    virtual void dropMessage(ActorMsg &&msg) = 0;
    virtual void deferMessage(ActorMsg &&msg) = 0;

protected:
    ActorId                             id_;
    /* Behaviors */
    Behavior                            initBehavior_;
    Behavior                            functionalBehavior_;
    Behavior                            stoppedBehavior_;
    Behavior                            inErrBehavior_;
    Behavior                            *currentBehavior_;
};

using ActorPtr = std::shared_ptr<Actor>;

using ActorQueue = folly::NotificationQueue<ActorMsg>;

struct NotificationQueueActor : Actor, ActorQueue::Consumer {
    NotificationQueueActor();
    explicit NotificationQueueActor(folly::EventBase *eventBase);
    
    void setEventBase(folly::EventBase *eventBase);
    virtual void send(ActorMsg &&msg) override;
    virtual void messageAvailable(ActorMsg &&msg) override;

    virtual void dropMessage(ActorMsg &&msg) override;
    virtual void deferMessage(ActorMsg &&msg) override;

 protected:
    ActorQueue queue_;
};

}  // namespace actor
