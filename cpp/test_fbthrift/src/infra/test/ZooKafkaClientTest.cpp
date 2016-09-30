#include <infra/ZooKafkaClient.h>
#include <infra/gen/commontypes_types.h>
#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <folly/Format.h>
#include <folly/futures/Future.h>
#include <thread>

DEFINE_string(zkdir, "~/playground/cpp/test_fbthrift/zookeeper-3.4.9", "zookeeper directory");

struct ZKHelper {
    ZKHelper()
    {
        start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    ~ZKHelper()
    {
        stop();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    void start() {
        auto cmd = folly::sformat("{}/bin/zkServer.sh start", FLAGS_zkdir);
        int ret = std::system(cmd.c_str());
        ASSERT_TRUE(ret == 0);
    }
    void stop() {
        auto cmd = folly::sformat("{}/bin/zkServer.sh stop", FLAGS_zkdir);
        int ret = std::system(cmd.c_str());
        ASSERT_TRUE(ret == 0);
        std::system("rm -rf /tmp/zookeeper/*");
    }
};

TEST(ZooKafkaClient, init_without_zk)
{
    infra::ZooKafkaClient client("test", "localhost:2181");
    ASSERT_THROW(client.init(), infra::ZookeeperException);
}

TEST(ZooKafkaClient, basic_ops)
{
    ZKHelper h;
    infra::ZooKafkaClient client("test", "localhost:2181");
    ASSERT_NO_THROW(client.init());

    /* Ensure basic put get work */
    auto putResult = client.put("/keys", "keys");
    putResult.wait();
    ASSERT_FALSE(putResult.getTry().hasException());

    auto getResult = client.get("/keys");
    getResult.wait();
    ASSERT_FALSE(getResult.getTry().hasException());
    ASSERT_EQ(getResult.value().data, "keys");

    /* Add some children */
    auto res = client.put("/services", "")
        .then([&client]() {
                return client.put("/services/service1", "service1data");
              })
        .then([&client]() {
                return client.put("/services/service2", "service2data");
              });
    res.wait();
    ASSERT_FALSE(res.getTry().hasException());

    /* Ensure get children works */
    auto children = client.getChildrenSync("/services");
    ASSERT_EQ(children.size(), 2);
    auto itr = std::find_if(children.begin(), children.end(),
                 [](const infra::CoordinationClient::KVPair &kv) { return kv.first == "service1";});
    ASSERT_TRUE(itr != children.end());
    itr = std::find_if(children.begin(), children.end(),
                       [](const infra::CoordinationClient::KVPair &kv) { return kv.first == "service2";});
    ASSERT_TRUE(itr != children.end());
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
