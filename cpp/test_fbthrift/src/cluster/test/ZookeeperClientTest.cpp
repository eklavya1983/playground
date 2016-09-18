#include <cluster/ZookeeperClient.h>
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

TEST(ZookeeperClient, init_without_zk)
{
    cluster::ZookeeperClient client("test", "localhost:2181");
    ASSERT_THROW(client.init(), cluster::ZookeeperException);
}

TEST(ZookeeperClient, init)
{
    ZKHelper h;
    cluster::ZookeeperClient client("test", "localhost:2181");
    ASSERT_NO_THROW(client.init());

    auto putResult = client.put("/keys", "keys");
    putResult.wait();
    ASSERT_FALSE(putResult.getTry().hasException());

    auto getResult = client.get("/keys");
    getResult.wait();
    ASSERT_FALSE(getResult.getTry().hasException());
    ASSERT_EQ(getResult.value(), "keys");
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
