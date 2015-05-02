#include <vector>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>
#include <folly/io/async/EventBaseManager.h>
#include <util/Log.h>
#include <actor/ActorSystem.hpp>
#include <util/TypeMappings.h>

using namespace actor;
using namespace folly;

struct PingPongActor {
};

TEST(ActorSystem, spawn) {
    ActorSystemPtr system(new ActorSystem("test", 9000,
                                         "127.0.0.1", 8000));
    system->init();

    auto eb = EventBaseManager::get()->getEventBase();
    eb->tryRunAfterDelay(std::bind(&EventBase::terminateLoopSoon, eb), 1000);
    system->loop();
}

TEST(ActorSystem, DISABLED_spawnmany) {
    std::vector<ActorSystemPtr> rootArray;
    for (int i = 0; i < 10; i++) {
        rootArray.push_back(ActorSystemPtr(new ActorSystem("test", 9000 + i ,
                                                           "127.0.0.1", 8000)));
        rootArray[i]->init();
    }
    sleep(1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    bhoomi::initActorMsgMappings();
    auto ret = RUN_ALL_TESTS();
    return ret;
}
