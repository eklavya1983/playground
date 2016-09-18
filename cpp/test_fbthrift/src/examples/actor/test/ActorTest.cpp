#include <actor/Actor.h>
#include <actor/ActorSystem.h>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>
#include <util/Log.h>
#include <util/TestUtils.h>

DECLARE_bool(failOnDrop);

using namespace actor;

struct TransToFunctional : util::DummySerializable {};
struct DoWork : util::DummySerializable {};

struct ActorTestF : testing::Test  {
    ActorTestF() {
        ADD_MSGMAPPING(Other,                       2);
        ADD_MSGMAPPING(Init,                        3);
        ADD_MSGMAPPING(DoWork,                      4);
        ADD_MSGMAPPING(TransToFunctional,           5);
    }
    ~ActorTestF() {
        clearActorMappings();
    }

    int initedCnt = 0;
    int workDoneCnt = 0;
};

struct MyActor : NotificationQueueActor {
    MyActor(ActorSystem *system, folly::EventBase *eventBase, ActorTestF *test)
    : NotificationQueueActor(system, eventBase),
      test_(test)
      {}

    static const char* className() { 
        return "MyActor";
    } 

protected:
    virtual void initBehaviors_() override {
        Actor::initBehaviors_();

        initBehavior_ += {
            on(Init) >> [this]() {
                test_->initedCnt++;
            },
            on(TransToFunctional) >> [this]() {
                changeBehavior(&functionalBehavior_);
            }
        };
        functionalBehavior_ += {
            on(DoWork) >> [this]() {
                test_->workDoneCnt++;
            }
        };
    }

    ActorTestF *test_;
};


TEST_F(ActorTestF, send) {
    folly::EventBase eventBase;
    ActorId id(apache::thrift::FRAGILE, 10, 10);
    MyActor a(nullptr, &eventBase, this);
    a.setId(id);
    a.init();
    a.dumpBehaviors();
    /* We do loopOnce twice here because in NotificationQueueActor::init() we
     * start consuming on eventbase.  So we need to loop twice
     */
    eventBase.loopOnce();
    eventBase.loopOnce();

    ASSERT_TRUE(a.isInitBehavior());
    ASSERT_TRUE(a.droppedCntr == 0);
    ASSERT_TRUE(a.deferredCntr == 0);
    ASSERT_TRUE(initedCnt == 1);
    ASSERT_TRUE(a.myId() == id);

    /* Send do work in init state.  It shoulnd't any effect */
    auto msg = makeActorMsg<DoWork>();
    ACTOR_SEND(&a, std::move(msg));
    eventBase.loopOnce();
    ASSERT_TRUE(a.isInitBehavior());
    ASSERT_TRUE(a.droppedCntr == 1);
    ASSERT_TRUE(a.deferredCntr == 0);

    /* Send TransToFunctional message.  Behavior should be set to functional */
    msg = makeActorMsg<TransToFunctional>();
    ACTOR_SEND(&a, std::move(msg));
    eventBase.loopOnce();
    ASSERT_TRUE(a.isFunctionalBehavior());
    ASSERT_TRUE(a.droppedCntr == 1);
    ASSERT_TRUE(a.deferredCntr == 0);
    ASSERT_TRUE(workDoneCnt == 0);

    /* Send do work in functional state */
    msg = makeActorMsg<DoWork>();
    ACTOR_SEND(&a, std::move(msg));
    eventBase.loopOnce();
    ASSERT_TRUE(a.isFunctionalBehavior());
    ASSERT_TRUE(a.droppedCntr == 1);
    ASSERT_TRUE(a.deferredCntr == 0);
    ASSERT_TRUE(workDoneCnt == 1);

    /* sent TransToStopped message.  Behavior should be set to stopped */
    /* Send do work message.  There shouldn't be any effect */
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    FLAGS_failOnDrop = false;
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
