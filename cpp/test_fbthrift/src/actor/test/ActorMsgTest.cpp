
#include <vector>
#include <actor/ActorMsg.h>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>

#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <actor/gen-cpp2/Service_types.h>

using namespace actor;

TEST(ActorMsg, mapping) {

    addActorMsgMapping<Init>(ActorMsgTypeIds::InitMsg);
    ASSERT_TRUE(gMsgMapTbl == nullptr ||
                gMsgMapTbl->find(ActorMsgTypeInfo<Init>::typeId) == gMsgMapTbl->end());

    addActorMsgMapping<GetActorRegistry>(ActorMsgTypeIds::GetActorRegistryMsg);
    ASSERT_DEATH(
        addActorMsgMapping<GetActorRegistry>(ActorMsgTypeIds::GetActorRegistryMsg),
        ".* registered");
}

TEST(ActorMsg, serialize) {
    using namespace cpp2;

    /* Register message mapping for serialization */
    addActorMsgMapping<Register>(ActorMsgTypeIds::RegisterMsg);

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
    auto typeId = ActorMsgTypeInfo<Register>::typeId;
    (gMsgMapTbl->at(typeId).first)(m, serializedBuf);
    
    /* Deserialize */
    ActorMsg m2;
    (gMsgMapTbl->at(typeId).second)(serializedBuf, m2);

    ASSERT_EQ(*payload, m2.payload<Register>());
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
