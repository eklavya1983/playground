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

TEST(ActorSystem, spawnManyActors) {
    ActorSystemPtr system(new ActorSystem(2));
    system->init();

    /* Spawn three actors */
    std::vector<ActorPtr> actors;
    for (int i = 0; i < 10; ++i) {
        auto a = system->spawnActor<PingPongActor>();
        actors.push_back(a);
        ASSERT_EQ(a->myId().systemId, system->myId().systemId);
        ASSERT_TRUE(a->myId().localId > system->myId().localId);
    }
    for (auto &a : actors) {
        auto foundA = system->lookUpActor(a->myId());
        ASSERT_EQ(a.get(), foundA.get());
    }
}

TEST(ActorSystem, sendSync) {
    ActorSystemPtr system(new ActorSystem(2));
    system->init();
    auto a = system->spawnActor<PingPongActor>();
    for (int i = 0; i < 100; ++i) {
        a->sendSync<Ping, Pong>(std::make_shared<Ping>());
    }
}

TEST(ActorSystem, messageSendAndReply) {
    ActorSystemPtr system(new ActorSystem(2));
    system->init();

    auto a1 = system->spawnActor<PingPongActor>();
    auto a2 = system->spawnActor<PingPongActor>();
    int cnt = 10;
    for (int i = 0; i < cnt; ++i) {
        /* Have a1 send ping message to a2 */
        auto msg = makeActorMsg<SendPing>();
        auto payload = std::make_shared<SendPing>();
        payload->to = a2->myId();
        msg.payload(payload);
        ACTOR_SEND(a1, std::move(msg));
    }
    sleep((cnt / 100) + 1);
    ASSERT_EQ(a1->pingCnt, 0);
    ASSERT_EQ(a1->pongCnt, cnt);
    ASSERT_EQ(a1->trackedPongCnt, 0);
    ASSERT_EQ(a2->pingCnt, cnt);
    ASSERT_EQ(a2->pongCnt, 0);
    ASSERT_EQ(a2->trackedPongCnt, 0);
}

TEST(ActorSystem, ActorRequest) {
    ActorSystemPtr system(new ActorSystem(2));
    system->init();

    auto a1 = system->spawnActor<PingPongActor>();
    auto a2 = system->spawnActor<PingPongActor>();
    int cnt = 10;
    for (int i = 0; i < cnt; ++i) {
        /* Have a1 send ping message to a2 */
        auto msg = makeActorMsg<SendPingReq>();
        auto payload = std::make_shared<SendPing>();
        payload->to = a2->myId();
        msg.payload(payload);
        ACTOR_SEND(a1, std::move(msg));
    }
    sleep((cnt / 100) + 1);
    ASSERT_EQ(a1->pingCnt, 0);
    ASSERT_EQ(a1->pongCnt, 0);
    ASSERT_EQ(a1->trackedPongCnt, cnt);
    ASSERT_EQ(a2->pingCnt, cnt);
    ASSERT_EQ(a2->pongCnt, 0);
    ASSERT_EQ(a2->trackedPongCnt, 0);
}

void initMappings() {
    initActorSystemMappings();
    ADD_MSGMAPPING(Ping, 100);
    ADD_MSGMAPPING(Pong, 101);
    ADD_MSGMAPPING(SendPing, 102);
    ADD_MSGMAPPING(SendPingReq, 103);
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    initMappings();
    auto ret = RUN_ALL_TESTS();
    return ret;
}
