#include <actor/Actor.h>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>
#include <util/TestUtils.h>

using namespace actor;

template<class T>
ActorMsgTypeId ActorMsgTypeInfo<T>::typeId;

enum {
    TransToFunctionalMsg = static_cast<int>(ActorMsgTypeIds::NOPAYLOAD_MSG_END) + 1,
    DoWorkMsg
};
struct TransToFunctional : util::DummySerializable {};
struct DoWork : util::DummySerializable {};

struct ActorTestF : testing::Test  {
    ActorTestF() {
        addActorMsgMapping<DoWork>(DoWorkMsg);
        addActorMsgMapping<TransToFunctional>(TransToFunctionalMsg);
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

protected:
    virtual void initBehaviors_() override {
        Actor::initBehaviors_();

        initBehavior_ += {
            on(Init) >> [this]() {
                test_->initedCnt++;
            },
            on(TransToFunctional) >> [this]() {
                changeBhavior(&functionalBehavior_);
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
    MyActor a(nullptr, &eventBase, this);
    a.init();
    eventBase.loopOnce();
    ASSERT_TRUE(a.droppedCntr == 0);
    ASSERT_TRUE(a.deferredCntr == 0);
    ASSERT_TRUE(initedCnt == 1);

    /* Send do work in init state.  It shoulnd't any effect */
    auto msg = makeActorMsg<DoWork>();
    /* Send TransToFunctional message.  Behavior should be set to functional */
    /* Send do work in functional state */
    /* sent TransToStopped message.  Behavior should be set to stopped */
    /* Send do work message.  There shouldn't be any effect */
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
