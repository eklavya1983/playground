#pragma once

#include <string>
#include <folly/io/async/NotificationQueue.h>
#include <actor/ActorMsg.h>
#include <actor/Behavior.h>

namespace actor {
using namespace cpp2;

struct ActorSystem;
using ActorHandlerF = std::function<void (ActorMsg &&)>;

struct Actor {
    /* Lifecycle */
    explicit Actor(ActorSystem *system);
    virtual ~Actor();
    virtual void init();

    void setId(const ActorId &id);
    ActorId getId() const;

    virtual void changeBhavior(Behavior *behavior);

    virtual void send(ActorMsg &&msg) = 0;
    virtual void handle(ActorMsg &&msg);

    virtual void dropMessage(ActorMsg &&msg) = 0;
    virtual void deferMessage(ActorMsg &&msg) = 0;

    template <class MsgT>
    void reply(const ActorMsg& msg, Payload &&payload);
    
protected:
    virtual void initBehaviors_();

    ActorSystem                         *system_;
    ActorId                             id_;
    std::string                         strId_;
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
    using Actor::Actor;
    explicit NotificationQueueActor(ActorSystem *system, folly::EventBase *eventBase);
    virtual void init() override;
    
    void setEventBase(folly::EventBase *eventBase);
    virtual void send(ActorMsg &&msg) override;
    virtual void messageAvailable(ActorMsg &&msg) override;

    virtual void dropMessage(ActorMsg &&msg) override;
    virtual void deferMessage(ActorMsg &&msg) override;

 protected:
    ActorQueue queue_;
};

}  // namespace actor
