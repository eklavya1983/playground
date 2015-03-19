/*
 * Copyright 2014 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #include <gtest/gtest.h>
#include <gen-cpp2/TestService.h>
#include <thrift/lib/cpp2/server/ThriftServer.h>
#include <thrift/lib/cpp2/async/HeaderClientChannel.h>
#include <thrift/lib/cpp2/async/RequestChannel.h>

#include <thrift/lib/cpp/util/ScopedServerThread.h>
#include <thrift/lib/cpp/async/TEventBase.h>
#include <thrift/lib/cpp/async/TAsyncSocket.h>
#include <thrift/lib/cpp/async/TAsyncServerSocket.h>

#include <thrift/lib/cpp2/async/StubSaslClient.h>
#include <thrift/lib/cpp2/async/StubSaslServer.h>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>

using namespace apache::thrift;
using namespace apache::thrift::test::cpp2;
using namespace apache::thrift::util;
using namespace apache::thrift::async;
using namespace apache::thrift::transport;
using apache::thrift::protocol::TBinaryProtocolT;

DEFINE_int32(nReqs, 1000, "Number of requests");
DEFINE_int32(maxOutstanding, 10, "Number of requests outstanding");
DEFINE_int32(clients, 1, "Number of clients");

class TestInterface : public TestServiceSvIf {

    virtual void reqResp(std::string& _return, std::unique_ptr<std::string> req) {
        _return = "how are you";
    }
};

std::shared_ptr<ThriftServer> getServer(bool useSimpleThreadManager = true) {
  std::shared_ptr<ThriftServer> server(new ThriftServer);
  if (useSimpleThreadManager) {
    std::shared_ptr<apache::thrift::concurrency::ThreadFactory> threadFactory(
        new apache::thrift::concurrency::PosixThreadFactory);
    std::shared_ptr<apache::thrift::concurrency::ThreadManager> threadManager(
        apache::thrift::concurrency::ThreadManager::newSimpleThreadManager(
            1, 1000, false, 0));
    threadManager->threadFactory(threadFactory);
    threadManager->start();
    server->setThreadManager(threadManager);
  }
  server->setPort(0);
  server->setInterface(std::unique_ptr<TestInterface>(new TestInterface));
  return server;
}

void AsyncCpp2Test() {
    folly::LifoSem sem(FLAGS_maxOutstanding);
    ScopedServerThread sst(getServer());

    TEventBase base;

    std::shared_ptr<TAsyncSocket> socket(
        TAsyncSocket::newSocket(&base, *sst.getAddress()));

    auto client_channel = HeaderClientChannel::newChannel(socket);
    TestServiceAsyncClient client(std::move(client_channel));

    
    std::thread eventThread([&]() {
        boost::polymorphic_downcast<HeaderClientChannel*>(
        client.getChannel())->setTimeout(100000);
        base.loopForever();
    });

    sleep(1);

    int completedReqs = 0;
    int nReqs = FLAGS_nReqs;
    auto start = std::chrono::steady_clock::now();
    auto sendFunc = [&]() {
        std::string req = "hello";
        client.reqResp([&](ClientReceiveState&& state) {
            std::string response;
            try {
                TestServiceAsyncClient::recv_reqResp(response, state);
            } catch(const std::exception& ex) {
            }
            completedReqs++;
            if (completedReqs == nReqs) {
                auto dur = std::chrono::steady_clock::now() - start;
                std::cout << "Time taken: " << dur.count();
                base.terminateLoopSoon();
            }
            std::cout << "Completed: " << completedReqs << std::endl;
            // sem.post();
        }, req);
    };
    for (int i = 0; i < nReqs; i++) {
        base.runInEventBaseThread(sendFunc);
        // sem.wait();
    }

    std::cout << "Exiting\n";

    eventThread.join();
}

#if 0
TEST(ThriftServer, InsecureAsyncCpp2Test) {
  AsyncCpp2Test(false);
}
#endif

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
#if 0
  testing::InitGoogleTest(&argc, argv);
  google::InitGoogleLogging(argv[0]);

  return RUN_ALL_TESTS();
#endif

    AsyncCpp2Test();
    return 0;
}
