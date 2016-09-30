#pragma once

#include <string>
#include <vector>

namespace folly {
template <class T>
class Future;
struct Unit;
}

namespace infra {

struct VersionedData;

struct CoordinationClient {
    using KVPair = std::pair<std::string, VersionedData>;

    virtual void init() = 0;
    virtual folly::Future<VersionedData> get(const std::string &key) = 0;
    // virtual folly::Future<std::vector<KVPair>> getChildren(const std::string &key) = 0;
    virtual std::vector<KVPair> getChildrenSync(const std::string &key) = 0;
};

}  // namespace infra
