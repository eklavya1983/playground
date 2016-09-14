#pragma once

#include <memory>
#include <KVStore.h>

namespace cluster {

struct ZookeeperClient;
using ZookeeperClientSPtr = std::shared_ptr<ZookeeperClient>;

struct ZookeeperKVStore : KVStore {
    explicit ZookeeperKVStore(const ZookeeperClientSPtr &zkClient);
};

}  // namespace
