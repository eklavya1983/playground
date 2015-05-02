#pragma once

#include <string>
#include <folly/io/async/NotificationQueue.h>
#include <actor/ActorMsg.h>
#include <actor/Behavior.h>
#include <actor/RingBuffer.h>

/**
* @brief Use these macros for sending actor messages.  Using this macro helps us
* log/trace actor messages
*/
#define ACTOR_SEND(__actor__, __msg__) \
    do { \
        ActorMsg &&__msgRef__ = (__msg__); \
        VLOG(LMSG) << "send :" << __msgRef__; \
        (__actor__)->send(std::move(__msgRef__)); \
    } while(false)

/**
* @brief For tracing actor messages
*/
#define ACTOR_MSG_TRACE(__msg__) traceBuffer_.push(__msg__.header())

namespace actor {
using namespace cpp2;

struct ActorSystem;
using ActorHandlerF = std::function<void (ActorMsg &&)>;

struct Actor;
using ActorPtr = std::shared_ptr<Actor>;

struct Actor : std::enable_shared_from_this<Actor> {
    /* Lifecycle */
    explicit Actor(ActorSystem *system);
    virtual ~Actor();
    virtual void init();
    ActorPtr getPtr();

    void setId(const ActorId &id);
    inline const ActorId& myId() const {return myId_;}

    virtual void changeBehavior(Behavior *behavior);

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

    /* Behavior related */
    bool isInitBehavior() const { return currentBehavior_ == &initBehavior_; }
    bool isFunctionalBehavior() const { return currentBehavior_ == &functionalBehavior_; }
    void dumpBehaviors();

    /* Exposed so that message to behavior logging is more useful.  Make sure every
     * subclass of Actor overrides it
     */
    static const char* className() { 
        return "Actor";
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
    Behavior                            *currentBehavior_;
    /* For tracing messages. Messages are traced when they are handled not when they are sent
     * to avoid having to lock
     */
    RingBuffer<ActorMsgHeader, 32>      traceBuffer_;
};

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
    folly::EventBase *eventBase_;
};

#if 0
struct GroupActor : NotificationQueueActor {
    using NotificationQueueActor::NotificationQueueActor;

 protected:
    virtual void initBehaviors_();
};
#endif
}  // namespace actor
