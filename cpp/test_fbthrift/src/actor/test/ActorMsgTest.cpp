
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

TEST(ActorMsg, serialize) {
    using namespace cpp2;
    std::unique_ptr<folly::IOBuf> serializedBuf;
    ActorMsgHeader h;
    ActorMsgHeader h2;
    h.requestId = 100;

    serializeActorMsg<ActorMsgHeader>(h, serializedBuf);

    deserializeActorMsg<ActorMsgHeader>(serializedBuf.get(),  h2);

    ASSERT_EQ(h.requestId, h2.requestId);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
