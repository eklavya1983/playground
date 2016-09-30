#include <zookeeper.h>
#include <infra/ZooKafkaClient.h>
#include <infra/gen/commontypes_types.h>
#include <chrono>
#include <thread>
#include <util/Log.h>
#include <folly/futures/Future.h>
#include <folly/Format.h>

namespace infra {

using StringPromise = folly::Promise<std::string>;
using StringVectorPromise = folly::Promise<std::vector<std::string>>;
using VersionedDataPromise = folly::Promise<VersionedData>;
using VersionedDataVectorPromise = folly::Promise<std::vector<VersionedData>>;

static void stringVectorCompletionCb(int rc,
                                     const String_vector *strings,
                                     const void *data)
{
    StringVectorPromise *promise = const_cast<StringVectorPromise*>(reinterpret_cast<const StringVectorPromise*>(data));

    if (rc != ZOK) {
        LOG(WARNING) << "stringVectorCompletionCb error: " << zerror(rc);
        promise->setException(infra::ZookeeperException(rc));
    } else {
        std::vector<std::string> resp;
        for (int i = 0; strings && i < strings->count; i++) {
            resp.push_back(std::string(strings->data[i]));
        }
        promise->setValue(std::move(resp));
    }
    delete promise;
}

static void stringCompletionCb(int rc,
                               const char *value,
                               const void *data)
{
    StringPromise *promise = const_cast<StringPromise*>(reinterpret_cast<const StringPromise*>(data));

    if (rc != ZOK) {
        LOG(WARNING) << "stringCompletionCb error: " << zerror(rc);
        promise->setException(infra::ZookeeperException(rc));
    } else {
        promise->setValue(std::string(value));
    }
    delete promise;
}

static void dataCompletionCb(int rc, const char *value, int value_len,
                             const Stat *stat, const void *data)
{
    VersionedDataPromise *promise = const_cast<VersionedDataPromise*>(reinterpret_cast<const VersionedDataPromise*>(data));
    if (rc != ZOK) {
        LOG(WARNING) << "dataCompletionCb error: " << zerror(rc);
        promise->setException(infra::ZookeeperException(rc));
    } else {
        VersionedData data;
        data.version = stat->czxid;
        data.data = std::move(std::string(value, value_len));
        promise->setValue(data);
    }
    delete promise;
}

void ZooKafkaClient::watcherFn(zhandle_t *zh,
                                int type,
                                int state,
                                const char *path,
                                void *watcherCtx)
{
    static_cast<ZooKafkaClient*>(watcherCtx)->watcher(type, state, path);
}



ZooKafkaClient::ZooKafkaClient(const std::string &logContext,
                                 const std::string& servers)
    : logContext_(logContext),
    servers_(servers)
{
}

ZooKafkaClient::~ZooKafkaClient()
{
    int rc = zookeeper_close(zh_);
    if (rc != ZOK) {
        CLog(WARNING) << "Failed to close zookeeper client.  error: " << zerror(rc);
    }
}

void ZooKafkaClient::init()
{
    zh_ = zookeeper_init(servers_.c_str(), &ZooKafkaClient::watcherFn, 30000, 0, this, 0);
    if (zh_ == nullptr) {
        CLog(FATAL) << "Failed to initialize zookeeper instance";
        throw ZookeeperException(ZINVALIDSTATE );
    }
    // TODO(Rao): We don't need to do this.  Connection can take place in the
    // backgroud
    blockUntilConnectedOrTimedOut_(5);

}

void ZooKafkaClient::blockUntilConnectedOrTimedOut_(int seconds)
{
    int secondsElapsed = 0;
    while (zoo_state(zh_) != ZOO_CONNECTED_STATE && secondsElapsed < seconds) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        secondsElapsed += 2;
    }

    if (zoo_state(zh_) != ZOO_CONNECTED_STATE) {
        CLog(ERROR) << "Failed to connect to zookeeper servers";
        throw ZookeeperException(ZINVALIDSTATE );
    }
}

void ZooKafkaClient::close()
{
    zookeeper_close(zh_);
}

void ZooKafkaClient::watcher(int type, int state, const char *path)
{
    CLog(INFO) << "type:" << typeToStr(type)
        << " state:" << stateToStr(state)
        << " path:" << path;
}

std::string ZooKafkaClient::typeToStr(int type)
{
    if (type == ZOO_CREATED_EVENT)
        return "CREATED_EVENT";
    if (type == ZOO_DELETED_EVENT)
        return "DELETED_EVENT";
    if (type == ZOO_CHANGED_EVENT)
        return "CHANGED_EVENT";
    if (type == ZOO_CHILD_EVENT)
        return "CHILD_EVENT";
    if (type == ZOO_SESSION_EVENT)
        return "SESSION_EVENT";
    if (type == ZOO_NOTWATCHING_EVENT)
        return "NOTWATCHING_EVENT";

    return "UNKNOWN_EVENT_TYPE";
}

std::string ZooKafkaClient::stateToStr(int state)
{
    if (state == 0)
        return "CLOSED_STATE";
    if (state == ZOO_CONNECTING_STATE)
        return "CONNECTING_STATE";
    if (state == ZOO_ASSOCIATING_STATE)
        return "ASSOCIATING_STATE";
    if (state == ZOO_CONNECTED_STATE)
        return "CONNECTED_STATE";
    if (state == ZOO_EXPIRED_SESSION_STATE)
        return "EXPIRED_SESSION_STATE";
    if (state == ZOO_AUTH_FAILED_STATE)
        return "AUTH_FAILED_STATE";

    return "INVALID_STATE";
}

folly::Future<std::string> ZooKafkaClient::put(const std::string &key,
                                                const std::string &value)
{
    StringPromise *promise = new StringPromise();
    auto future = promise->getFuture();
    auto rc = zoo_acreate(zh_, key.c_str(), value.c_str(), value.length(),
                              &ZOO_OPEN_ACL_UNSAFE, 0, &stringCompletionCb, promise);
    if (rc != ZOK) {
        CLog(WARNING) << "failed to put key: " << key << " error: " << zerror(rc);
        delete promise;
        return folly::makeFuture<std::string>(ZookeeperException(rc));
    }
    return future;
}

folly::Future<VersionedData> ZooKafkaClient::get(const std::string &key)
{
    VersionedDataPromise *promise = new VersionedDataPromise();
    auto future = promise->getFuture();
    auto rc = zoo_awget(zh_, key.c_str(), nullptr, nullptr,
                        &dataCompletionCb, promise);
    if (rc != ZOK) {
        CLog(WARNING) << "failed to get key: " << key << " error: " << zerror(rc);
        delete promise;
        return folly::makeFuture<VersionedData>(ZookeeperException(rc));
    }
    return future;
}

folly::Future<std::vector<std::string>>
ZooKafkaClient::getChildrenSimple(const std::string &key)
{
    StringVectorPromise *promise = new StringVectorPromise();
    auto future = promise->getFuture();
    auto rc = zoo_awget_children(zh_, key.c_str(), nullptr, nullptr,
                                 &stringVectorCompletionCb, promise);
    if (rc != ZOK) {
        CLog(WARNING) << "failed to get children key: " << key << " error: " << zerror(rc);
        delete promise;
        return folly::makeFuture<std::vector<std::string>>(ZookeeperException(rc));
    }
    return future;
}

#if 0
folly::Future<std::vector<KVPair>>
ZooKafkaClient::getChildren(const std::string &key)
{
    auto f = getChildrenSimple(key);
    f.then([this, key](const std::vector<std::string>& children) {
           std::vector<folly::Future<VersionedData>>fs;
           for (const auto &c : children) {
              fs.push_back(this->get(folly::sformat("{}/{}", key, c)));
           }
           return folly::collect(fs);
           // TODO: Map
    })
}
#endif

std::vector<CoordinationClient::KVPair> ZooKafkaClient::getChildrenSync(const std::string &key)
{
    std::vector<CoordinationClient::KVPair> ret;
    auto children  = getChildrenSimple(key).get();
    for (const auto &c : children) {
        auto data = this->get(folly::sformat("{}/{}", key, c)).get();
        ret.push_back(std::make_pair(c, data));
    }
    return ret;
}

}  // namespace infra
