#include <util/Log.h>
#include <actor/ActorUtil.h>
#include <actor/Actor.h>
#include <actor/ActorSystem.h>
#include <actor/ActorRequest.h>
#include <sstream>

DEFINE_bool(failOnDrop, true, "fail on dropping messages");

namespace actor {

Actor::Actor(ActorSystem *system)
: initBehavior_("init"),
  functionalBehavior_("functional")
{
    system_ = system;
    myId_ = ActorSystem::invalidActorId();
    currentBehavior_ = nullptr;
    droppedCntr = 0;
    deferredCntr = 0;
    tracker_ = nullptr;
}

Actor::~Actor() {
    ALog(INFO) << "Actor destructed";
}

void Actor::init() {
    ALog(INFO) << "Actor inited";
    initBehaviors_();
    dumpBehaviors();
    if (currentBehavior_ == nullptr) {
        currentBehavior_ = &initBehavior_;
        ACTOR_SEND(this, makeActorMsg<Init>());
    }
}

ActorPtr Actor::getPtr() {
    return shared_from_this();
}

void Actor::initBehaviors_() {
    initBehavior_ = {
        onresp(Other) >> [this]() {
            if (tracker_) {
                tracker_->handleResponse(std::move(*curMsg_));
                curMsg_ = nullptr;
            }
        },
        on(Other) >> [this]() {
            ALog(INFO) << "Init - dropping messages";
            dropMsg();
        }
    };
    functionalBehavior_ = {
        on(Other) >> [this]() {
            ALog(INFO) << "functional - dropping messages";
            dropMsg();
        }
    };
}

void Actor::setId(const ActorId &id) {
    myId_ = id;

    /* Set the string used for logging as well */
    std::stringstream ss;
    ss << "[" << id.systemId << ":" << id.localId << "]";
    strId_ = ss.str();
}

void Actor::changeBehavior(Behavior *behavior) {
    currentBehavior_ = behavior;
}

void Actor::handle(ActorMsg &&msg) {
    /* Cache current message.  This cached message is used in all the behavior
     * handlers
     */
    curMsg_ = &msg;

    DCHECK(to() == myId());

    AVLog(LMSG) << "handle: " << msg;
    ACTOR_MSG_TRACE(msg);

    // TODO: It'd be nice to log which behiavior is handling the messsage as well
    currentBehavior_->handle({msg.direction(), msg.typeId()});

    curMsg_ = nullptr;
}

void Actor::dumpBehaviors()
{
    AVLog(LMSG) << "Actor behaviors: \n"
        << initBehavior_
        << functionalBehavior_;
}

NotificationQueueActor::NotificationQueueActor(ActorSystem *system,
                                               folly::EventBase *eventBase)
: Actor(system)
{
    setEventBase(eventBase);
}

NotificationQueueActor::~NotificationQueueActor() {
    delete tracker_;
}

void NotificationQueueActor::init() {
    Actor::init();

    /* Creating the tracker here.  Ideally w'd have one tracker per eventbase and it
     * will be set by the ActorSystem.
     * For simplicity we'll have a tracker per actor.  
     */
    tracker_ = new RequestTracker(system_, eventBase_);

    ALog(INFO) << " Eventbase set to: " << eventBase_;
    CHECK(eventBase_ != nullptr); 
    eventBase_->runInEventBaseThread([this] {
        startConsuming(eventBase_, &queue_);
    });
}

void NotificationQueueActor::setEventBase(folly::EventBase *eventBase) {
    eventBase_ = eventBase;
}

folly::EventBase* NotificationQueueActor::getEventBase() {
    return eventBase_;
}

void NotificationQueueActor::send(ActorMsg &&msg) {
    DCHECK(msg.direction() == MSGDIRECTION_NORMAL ||
           msg.direction() == MSGDIRECTION_RESPONSE);
    DCHECK(msg.typeId() != ActorMsg::INVALID_MSGTYPEID);

    msg.to(myId());

    queue_.putMessage(std::move(msg));
}

void NotificationQueueActor::messageAvailable(ActorMsg &&msg) {
    handle(std::move(msg));
}

void NotificationQueueActor::dropMsg() {
    droppedCntr++;
    ALog(WARNING) << currentBehavior_->name() << " Dropped message: " << *curMsg_;
    CHECK(!FLAGS_failOnDrop) << "Dropping messages";
    // CHECK(false) << "Dropping messages";
    // TODO: Implement
}

void NotificationQueueActor::deferMsg() {
    deferredCntr++;
    ALog(WARNING) << currentBehavior_->name() << "Deferred message: " << *curMsg_;
    // TODO: Implement
}

#if 0
void GroupActor::initBehaviors_() {
    initBehavior_ += {
        on(InitGroup) >> [this] {
        },
        on(CommitGroup) >> [] {
        }
    };
}
#endif

}  // namespace actor
