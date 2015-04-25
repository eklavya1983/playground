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
    inline const ActorId& myId() const {return myId_;}

    virtual void changeBhavior(Behavior *behavior);

    virtual void send(ActorMsg &&msg) = 0;
    virtual void handle(ActorMsg &&msg);

    virtual void dropMsg() = 0;
    virtual void deferMsg() = 0;

    template <class MsgT>
    void reply(Payload &&payload);

    inline ActorMsg* msg() {return curMsg_;}
    inline const ActorMsgTypeId& typeId() const {return curMsg_->typeId();}
    inline const ActorId& from() const {return curMsg_->from();}
    inline const ActorId& to() const {return curMsg_->to();}
    inline const RequestId& requestId() const {return curMsg_->requestId();}
    template <class T>
    inline T& payload() {
        return curMsg_->payload<T>();
    }

    /* Counters.  Exposed for testing */
    int32_t                             droppedCntr;
    int32_t                             deferredCntr;
protected:
    virtual void initBehaviors_();


    ActorSystem                         *system_;
    ActorId                             myId_;
    std::string                         strId_;
    /* Message related */
    ActorMsg                            *curMsg_;
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

    virtual void dropMsg() override;
    virtual void deferMsg() override;

 protected:
    ActorQueue queue_;
};

}  // namespace actor
