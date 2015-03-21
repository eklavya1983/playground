#include <vector>
#include <actor/ActorSystem.h>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>

using namespace actor;

TEST(ActorSystem, spawn) {
    std::vector<ActorSystemPtr> rootArray;
    for (int i = 0; i < 10; i++) {
        rootArray.push_back(ActorSystemPtr(new ActorSystem(9000 + i , "127.0.0.1", 8000)));
        rootArray[i]->init();
    }
    sleep(1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
