#include <actor/ActorRequest.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace actor;

TEST(RequestTracker, addRequest) {
    RequestTracker tracker;
    QuorumRequest& req = tracker.allocRequest<QuorumRequest>();
    req
    .withQuorum(2)
    .toActors({ActorId(), ActorId()});

}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
