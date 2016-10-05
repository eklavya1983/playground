#include <folly/futures/Future.h>
#include <folly/Format.h>
#include <infra/gen/commontypes_types.h>
#include <infra/gen/configtree_types.h>
#include <infra/ConnectionCache.h>
#include <infra/ModuleProvider.h>
#include <infra/CoordinationClient.h>
#include <infra/gen/commontypes_constants.h>
#include <infra/gen/configtree_constants.h>
#include <infra/gen-ext/commontypes_ext.h>
#include <infra/Serializer.tcc>

namespace infra {

struct ConnectionCache::CacheItem {
    CacheItem() {
        serviceVersion = g_commontypes_constants.INVALID_VERSION;
    }
    ServiceInfo         serviceInfo;
    int64_t             serviceVersion;
};

ConnectionCache::ConnectionCache(const std::string &logContext,
                                 ModuleProvider* provider)
{
    logContext_ = logContext;
    provider_ = provider;
}

void ConnectionCache::init()
{
    /* TODO(Rao): Register to receive service updates */
    auto f = [this](int64_t version, const std::string &payload) {
        ServiceInfo info;
        deserializeFromThriftJson<ServiceInfo>(payload, info, getLogContext());
        CLog(INFO) << "connection cache update version:" << version << " info:" << info;
        // TODO: Update cache in event base
    };
    provider_->getCoordinationClient()->\
        subscribeToTopic(g_configtree_constants.TOPIC_SERVICES, f);
}

std::string ConnectionCache::getConnectionId(const ServiceInfo& info)
{
    return folly::sformat("{}.{}", info.dataSphereId, info.id);
}

void ConnectionCache::fetchMyDatasphereEntries_()
{
    auto servicesRootPath = folly::sformat(g_configtree_constants.SERVICES_ROOT_PATH_FORMAT,
                                           provider_->getDatasphereId());
    /* We assume this function is called for service initialization context
     * That is why there is no locking necessary
     */
    CHECK(connections_.size() == 0);

    /* NOTE: This will block.  This is fine as long as we only do it from
     * service initialization context
     */
    auto serviceList = provider_->\
                       getCoordinationClient()->\
                       getChildrenSync(servicesRootPath);
    for (const auto &kv: serviceList) {
        auto &vData = kv.second;
        auto entry = std::make_shared<CacheItem>();
        entry->serviceVersion = vData.version;
        deserializeFromThriftJson<ServiceInfo>(vData.data, entry->serviceInfo, logContext_);
        connections_[getConnectionId(entry->serviceInfo)] = entry;

        CLog(INFO) << "added " << entry->serviceInfo << " version:" << entry->serviceVersion;
    }
}

}  // namespace infra
