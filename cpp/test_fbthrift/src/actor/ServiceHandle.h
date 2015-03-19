#pragma once

#include <folly/AtomicHashMap.h>

namespace actor {
struct ActorMsg;
}

namespace net {
using SvcHandleId = int64_t;

struct SvcHandle {
    SvcHandle();
    virtual ~SvcHandle();

    void sendActorReq(actor::ActorMsg &&msg);
};
using SvcHandlePtr = std::shared_ptr<SvcHandle>;

struct ServiceHandleCache {
    ServiceHandleCache(size_t sz);
    ~ServiceHandleCache();

    SvcHandlePtr getSvcHandle(const SvcHandleId &id);
    void registerSvcHandle(const SvcHandleId &id, const SvcHandlePtr &svcHandle);

 protected:
    folly::AtomicHashMap<SvcHandleId, SvcHandlePtr> svcHandleTbl_;
};
}  // namespace net
