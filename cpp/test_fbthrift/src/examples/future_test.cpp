#include <iostream>
#include <folly/futures/Future.h>

#include <glog/logging.h>
#include <gtest/gtest.h>


using namespace folly;
using namespace std;
int getInt();

thread_local int tInt = getInt();

int getInt() {
    return tInt;
}

TEST(thread, create) {
    tInt = 10;
    std::thread t([]() {
                  std::cout << "tInt: " << tInt;
                  });
    /*
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    */
    t.join();
}

TEST(Future, basic) {
    Promise<int> p;
    Future<int> f = p.getFuture();
    f
    .then([](int x) {
          cout << "lamda x: " << x << endl;
          throw std::runtime_error("error from lamda1");
    })
    .then([]() {
          cout << "lamda 2 " << endl;
    })
    .onError([](std::exception e) {
             cout << "exceptin: " << e.what();
     });
    cout << "Future chain made" << endl;

    cout << "fulfilling Promise" << endl;
    p.setValue(42);
    cout << "Promise fulfilled" << endl;
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}


