#include <glog/logging.h>
#include <ZookeeperClient.h>
#include <thread>

int main(int argc, const char* argv[]) {
    // google::InitGoogleLogging(argv[0]);
    cluster::ZookeeperClient client("localhost:2181");
    client.init();
    int len = 256;
    char buffer[256];
    int ret = zoo_wget(client.getHandle(),
                       "/domain",
                       nullptr,
                       nullptr,
                       buffer,
                       &len,
                       nullptr);
    CHECK(ret >= 0);
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
