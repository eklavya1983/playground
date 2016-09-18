#include <actor/ServiceHandle.h>

namespace net {
SvcHandle::SvcHandle() {
}

SvcHandle::~SvcHandle() {
}

void SvcHandle::sendActorReq(actor::ActorMsg &&msg) {
}

ServiceHandleCache::ServiceHandleCache(size_t sz)
    : svcHandleTbl_(sz)
{
}

ServiceHandleCache::~ServiceHandleCache() {
}

SvcHandlePtr ServiceHandleCache::getSvcHandle(const SvcHandleId &id) {
    auto itr = svcHandleTbl_.find(id);
    if (itr == svcHandleTbl_.end()) {
        return nullptr;
    }
    return itr->second;
}

void
ServiceHandleCache::registerSvcHandle(const SvcHandleId &id,
                                      const SvcHandlePtr &svcHandle) {
    svcHandleTbl_.insert(std::make_pair(id, svcHandle));
}

}
