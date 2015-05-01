#include <vector>

#include <util/Log.h>
#include <actor/ActorUtil.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>
#include <folly/io/async/EventBaseManager.h>
#include <util/Log.h>
#include <config/ConfigService.h>

using namespace actor;
using namespace config;

TEST(ActorSystem, spawn) {
    ActorSystemPtr system(new ConfigService("127.0.0.1", 8000));
    system->init();

    // ASSERT_EQ(system->myId(), ActorSystem::configActorId());

    auto eb = folly::EventBaseManager::get()->getEventBase();
    eb->tryRunAfterDelay(std::bind(&folly::EventBase::terminateLoopSoon, eb), 100);
    system->loop();
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    actor::initActorMsgMappings();
    auto ret = RUN_ALL_TESTS();
    return ret;
}
