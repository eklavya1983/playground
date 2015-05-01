
#include <vector>
#include <actor/ActorMsg.h>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>

#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <actor/gen-cpp2/Service_types.h>
#include <util/TypeMappings.h>

using namespace actor;

TEST(ActorMsg, mapping) {
    ADD_MSGMAPPING(Init, 2);
    ASSERT_TRUE(gMsgTypeInfoTbl != nullptr &&
                gMsgTypeInfoTbl->find(ActorMsgTypeEnum<Init>::typeId) != gMsgTypeInfoTbl->end());

    ADD_MSGMAPPING(GetActorRegistry, 3);
    #if 0
    ASSERT_DEATH(
        ADD_MSGMAPPING(GetActorRegistry, 3),
        ".* registered");
    #endif
}

TEST(ActorMsg, serialize) {
    using namespace cpp2;

    /* Register message mapping for serialization */
    ADD_MSGMAPPING(Register, 4);

    using apache::thrift::FRAGILE;
    ActorId from(FRAGILE, 10, 10);
    ActorId to(FRAGILE, 11, 11);
    auto payload = std::make_shared<Register>();
    payload->systemInfo.id = from;
    payload->systemInfo.ip = "127.0.0.1";
    payload->systemInfo.port = 8000;
    payload->systemInfo.type = "test";
    ActorMsg m = makeActorMsg<Register>(from, to, payload);
    
    /* Check makeActorMsg */
    ASSERT_EQ(m.from(), from);
    ASSERT_EQ(m.to(), to);
    ASSERT_EQ(m.payload<Register>(), *payload);

    /* Serialize */
    std::unique_ptr<folly::IOBuf> serializedBuf;
    auto typeId = ActorMsgTypeEnum<Register>::typeId;
    (gMsgTypeInfoTbl->at(typeId).serializer)(m, serializedBuf);
    
    /* Deserialize */
    ActorMsg m2;
    (gMsgTypeInfoTbl->at(typeId).deserializer)(serializedBuf, m2);

    ASSERT_EQ(*payload, m2.payload<Register>());
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
