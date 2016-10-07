#include <gtest/gtest.h>
#include <gflags/gflags.h>
#include <folly/Format.h>
#include <folly/futures/Future.h>
#include <thread>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <infra/Service.h>
#include <infra/gen/gen-cpp2/ServiceApi.h>
#include <infra/CoordinationClient.h>
#include <infra/StatusException.h>
#include <boost/cast.hpp>
#include <testlib/DatomBringupHelper.h>
#include <testlib/SignalUtils.h>
#include <infra/ZooKafkaClient.h>

using namespace apache::thrift::async;
using namespace apache::thrift;
using namespace infra;


TEST(Service, bringup)
{
    testlib::DatomBringupHelper bringupHelper;
    testlib::ScopedDatom d(bringupHelper);

    bringupHelper.addDataSphere("sphere1");

    ServiceInfo serviceInfo;
    serviceInfo.dataSphereId = "sphere1";
    serviceInfo.nodeId = "node1";
    serviceInfo.id = "service1";

    std::unique_ptr<Service> service1 (Service::newDefaultService(serviceInfo.id,
                                                                  serviceInfo,
                                                                  "localhost:2181"));
    /* Init without service being added to datasphere should fail */
    ASSERT_THROW(service1->init(), StatusException);

    bringupHelper.addService("sphere1", "node1", "service1", "127.0.0.1", 8080);
    service1.reset(Service::newDefaultService(serviceInfo.id,
                                              serviceInfo,
                                              "localhost:2181"));
    service1->init();

    /* Add another service */
    serviceInfo.id = "service2";
    bringupHelper.addService("sphere1", "node1", serviceInfo.id, "127.0.0.1", 8084);
    std::unique_ptr<Service> service2 (Service::newDefaultService(serviceInfo.id,
                                                                  serviceInfo,
                                                                  "localhost:2181"));
    service2->init();


    // testlib::waitForSIGINT();
}

TEST(Service, DISABLED_testserver)
{
    infra::ServiceInfo info;
    info.id = "service1";
    info.ip = "localhost";
    info.port = 8082;
    infra::Service *s = infra::Service::newDefaultService("test", info, "");
    s->init();
    std::thread t([s]() { s->run(); });
    std::this_thread::sleep_for(std::chrono::seconds(2));

    folly::EventBase base;
    std::shared_ptr<TAsyncSocket> socket(
        TAsyncSocket::newSocket(&base, "127.0.0.1", 8082));

    infra::cpp2::ServiceApiAsyncClient client(
        std::unique_ptr<HeaderClientChannel,
        folly::DelayedDestruction::Destructor>(
            new HeaderClientChannel(socket)));

    boost::polymorphic_downcast<HeaderClientChannel*>(
        client.getChannel())->setTimeout(10000);
    std::string response;
    auto f = client.future_getModuleState({});
    base.loop();
    ASSERT_TRUE(f.get() ==  "ok");

    client.sync_getModuleState(response, {});
    EXPECT_EQ(response, "ok");

    s->shutdown();
#if 0
    client.sync_getModuleState(response, {});
    EXPECT_EQ(response, "ok");

    RpcOptions options;
    options.setTimeout(std::chrono::milliseconds(1));
    try {
        // should timeout
        client.sync_sendResponse(options, response, 10000);
    } catch (const TTransportException& e) {
        EXPECT_EQ(int(TTransportException::TIMED_OUT), int(e.getType()));
        return;
    }
    ADD_FAILURE();
#endif

  t.join();
  delete s;
}

#if 0
struct Handler : infra::cpp2::ServiceApiSvIf 
{
  void getModuleState(std::string& _return, std::unique_ptr<std::map<std::string, std::string>> arguments) override {
      _return = "hello";
      LOG(INFO) << "returning hello";
  }
};

TEST(Service, init)
{
    //     infra::Service<> s("Service", "data", "localhost", 8080, "");
    // s.init();
    std::thread t([]() {
                  apache::thrift::ThriftServer* server_ = new apache::thrift::ThriftServer("disabled", false);
                  server_->setPort(8082);
                  server_->setInterface(std::make_shared<Handler>());
                  // server_->setIOThreadPool(system_->getIOThreadPool());
                  server_->setNWorkerThreads(2);
                  server_->serve();
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));

#if 0
    using namespace apache::thrift::transport;
    using apache::thrift::protocol::TBinaryProtocolT;
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>("localhost", 8082);
    socket->open();

    std::shared_ptr<TFramedTransport> transport =
        std::make_shared<TFramedTransport>(socket);
    std::shared_ptr<TBinaryProtocolT<TBufferBase>> protocol =
        std::make_shared<TBinaryProtocolT<TBufferBase>>(transport);
    auto client = std::make_shared<infra::ServiceApiClient>(protocol);
    std::string ret;
    client->getModuleState(ret, {});
#endif

    t.join();
}
#endif


int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
