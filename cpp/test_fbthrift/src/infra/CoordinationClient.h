#pragma once

#include <string>

namespace folly {
template <class T>
class Future;
struct Unit;
}

namespace infra {

struct VersionedData;

struct CoordinationClient {
    virtual void init() = 0;
    virtual folly::Future<VersionedData> get(const std::string &key) = 0;
    static std::string makePath(const std::initializer_list<std::string> &args);
};

}  // namespace infra
