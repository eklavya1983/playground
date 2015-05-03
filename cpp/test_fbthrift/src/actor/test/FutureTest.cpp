#include <folly/futures/Future.h>

#include <glog/logging.h>
#include <gtest/gtest.h>

using namespace folly;

TEST(Future, simple) {
    {
    auto f = makeFuture<int>(42)
        .then([](Try<int>&& t) {
              EXPECT_EQ(42, t.value());
              throw 54;
              return 54;
              })
        .then([](int t) {
              EXPECT_EQ(54, t);
              });
    }
    {
    auto f = makeFuture<int>(42)
        .then([](Try<int>&& t) {
              EXPECT_EQ(42, t.value());
              throw 54;
              return 54;
              })
        .onError([](int i) {
            return i;
        })
        .then([](Try<int>&& t) {
              EXPECT_EQ(54, t.value());
              });
    }
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
