#include <infra/KafkaClient.h>
#include <folly/futures/Future.h>
#include <librdkafka/rdkafkacpp.h>
#include <util/Log.h>
#include <infra/gen/status_types.h>
#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <infra/gen/status_types.h>

#if 0
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
#endif

TEST(KafkaClient, init)
{
    infra::KafkaClient client("service1", "localhost", "service1");
    client.init();
    for (int i = 0; i < 10024; i++) {
        auto status = client.publishMessage("test", "hello");
        ASSERT_TRUE(status == infra::STATUS_OK);
    }
}


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
