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
        on(1) >> [msg1called](ActorMsg &&m) mutable {
            msg1called  = true;
            std::cout << "Received one\n";
        },
        on(2) >> [msg2called](ActorMsg &&m) mutable {
            msg2called = true;
        }
    };

    ActorMsg msg1;
    setActorMsgType(msg1, 1);
    ActorMsg msg2;
    setActorMsgType(msg2, 2);

    b.handle(std::move(msg1));
    b.handle(std::move(msg2));

    ASSERT_TRUE(msg1called);
    ASSERT_TRUE(msg2called);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
