#include <glog/logging.h>
#include <gtest/gtest.h>
#include <actor/Behavior.h>
#include <iostream>

using namespace actor;

void behavior_test() {
    bool msg1called = false;
    bool msg2called = false;

    Behavior b;    
    b = {
        OnMsg(1) >> [msg1called]() mutable {
            msg1called  = true;
            std::cout << "Received one\n";
        },
        OnMsg(2) >> [msg2called]() mutable {
            msg2called = true;
        }
    };

    ActorMsg msg1;
    setActorMsgTypeId(msg1, 1);
    ActorMsg msg2;
    setActorMsgTypeId(msg2, 2);

    b.handle(actorMsgTypeId(msg1));
    b.handle(actorMsgTypeId(msg2));

    ASSERT_TRUE(msg1called);
    ASSERT_TRUE(msg2called);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
