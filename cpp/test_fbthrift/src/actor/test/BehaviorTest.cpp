#include <glog/logging.h>
#include <gtest/gtest.h>
#include <actor/Behavior.h>
#include <iostream>

using namespace actor;

const ActorMsgTypeId FooTypeId = 100;
const ActorMsgTypeId BarTypeId = 101;
const ActorMsgTypeId BlahTypeId = 102;

const char* className() {
    return "Temp";
}

TEST(Behavior, handle) {
    bool msg1called = false;
    bool msg2called = false;
    int anyMsgMatched = 0;
    int anyRespMsgMateched = 0;
    int fooRespMatched = 0;

    Behavior b("Behavior");
    b = {
        OnMsg({MSGDIRECTION_NORMAL, FooTypeId}) >> [&]() mutable {
            msg1called  = true;
            std::cout << "Received one\n";
        },
        OnMsg({MSGDIRECTION_NORMAL, BarTypeId}) >> [&]() mutable {
            msg2called = true;
        },
        OnMsg({MSGDIRECTION_RESPONSE, FooTypeId}) >> [&]() mutable {
            fooRespMatched++;
        },
        onresp(Other) >> [&]() mutable {
            anyRespMsgMateched++;
        },
        on(Other) >> [&]() mutable {
            anyMsgMatched++;
        }
    };

    ActorMsg msg1;
    msg1.typeId(FooTypeId);
    ActorMsg msg2;
    msg2.typeId(BarTypeId);

    b.handle({MSGDIRECTION_NORMAL, FooTypeId});
    ASSERT_TRUE(msg1called);

    b.handle({MSGDIRECTION_NORMAL, BarTypeId});
    ASSERT_TRUE(msg2called);

    b.handle({MSGDIRECTION_RESPONSE, FooTypeId});
    b.handle({MSGDIRECTION_RESPONSE, FooTypeId});
    ASSERT_EQ(fooRespMatched, 2);
    ASSERT_EQ(anyRespMsgMateched, 0);
    ASSERT_EQ(anyMsgMatched, 0);

    b.handle({MSGDIRECTION_RESPONSE, BarTypeId});
    b.handle({MSGDIRECTION_RESPONSE, BarTypeId});
    ASSERT_EQ(anyRespMsgMateched, 2);
    ASSERT_EQ(fooRespMatched, 2);
    ASSERT_EQ(anyMsgMatched, 0);

    b.handle({MSGDIRECTION_NORMAL, BlahTypeId});
    b.handle({MSGDIRECTION_NORMAL, BlahTypeId});
    ASSERT_EQ(anyMsgMatched, 2);
    ASSERT_EQ(anyRespMsgMateched, 2);
    ASSERT_EQ(fooRespMatched, 2);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    initCommonActorMsgMappings();
    auto ret = RUN_ALL_TESTS();
    return ret;
}
