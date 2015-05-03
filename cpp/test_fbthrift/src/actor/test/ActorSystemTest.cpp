#include <vector>

#include <glog/logging.h>
#include <gtest/gtest.h>
#include <utility>
#include <iostream>
#include <thread>
#include <folly/io/async/EventBaseManager.h>
#include <util/Log.h>
#include <actor/ActorSystem.hpp>
#include <actor/test/PingPongActor.h>
#include <util/TypeMappings.h>

using namespace actor;
using namespace folly;
using namespace actor::test;

TEST(ActorSystem, DISABLED_spawn) {
    ActorSystemPtr system(new ActorSystem());
    system->init();

    auto eb = EventBaseManager::get()->getEventBase();
    eb->tryRunAfterDelay(std::bind(&EventBase::terminateLoopSoon, eb), 1000);
    system->loop();
}

TEST(ActorSystem, DISABLED_spawnmany) {
    std::vector<ActorSystemPtr> rootArray;
    for (int i = 0; i < 10; i++) {
        rootArray.push_back(ActorSystemPtr(new ActorSystem()));
        rootArray[i]->init();
    }
    sleep(1);
}

TEST(ActorSystem, test) {
    ActorSystemPtr system(new ActorSystem());
    system->init();

    /* Spawn three actors */
    auto a1 = system->spawnActor<PingPongActor>();
    auto a2 = system->spawnActor<PingPongActor>();
    auto a3 = system->spawnActor<PingPongActor>();

    /* Have a1 send ping message to a2 */
    auto msg = makeActorMsg<SendPing>();
    auto payload = std::make_shared<SendPing>();
    payload->to = a2->myId();
    msg.payload(payload);
    ACTOR_SEND(a1, std::move(msg));
    sleep(2);
}

void initMappings() {
    initActorSystemMappings();
    ADD_MSGMAPPING(Ping, 100);
    ADD_MSGMAPPING(Pong, 101);
    ADD_MSGMAPPING(SendPing, 102);
    ADD_MSGMAPPING(SendQuorumPing, 103);
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    initMappings();
    auto ret = RUN_ALL_TESTS();
    return ret;
}
