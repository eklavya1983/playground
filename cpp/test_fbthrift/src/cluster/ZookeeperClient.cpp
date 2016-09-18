#include <cluster/ZookeeperClient.h>
#include <zookeeper.h>
#include <chrono>
#include <thread>
#include <util/Log.h>
#include <folly/futures/Future.h>

using StringPromise = folly::Promise<std::string>;

static void stringCompletionCb(int rc,
                               const char *value,
                               const void *data)
{
    StringPromise *promise = const_cast<StringPromise*>(reinterpret_cast<const StringPromise*>(data));

    if (rc != ZOK) {
        LOG(WARNING) << "stringCompletionCb error: " << zerror(rc);
        promise->setException(cluster::ZookeeperException(rc));
    } else {
        promise->setValue(std::string(value));
    }
    delete promise;
}
static void dataCompletionCb(int rc, const char *value, int value_len,
                             const Stat *stat, const void *data)
{
    StringPromise *promise = const_cast<StringPromise*>(reinterpret_cast<const StringPromise*>(data));
    if (rc != ZOK) {
        LOG(WARNING) << "dataCompletionCb error: " << zerror(rc);
        promise->setException(cluster::ZookeeperException(rc));
    } else {
        // std::string strVal(value, value_len);
        std::string strVal("keys", 4);
        promise->setValue(strVal);
    }
    delete promise;
}

namespace cluster {


void ZookeeperClient::watcherFn(zhandle_t *zh,
                                int type,
                                int state,
                                const char *path,
                                void *watcherCtx)
{
    static_cast<ZookeeperClient*>(watcherCtx)->watcher(type, state, path);
}



ZookeeperClient::ZookeeperClient(const std::string &logContext,
                                 const std::string& servers)
    : logContext_(logContext),
    servers_(servers)
{
}

ZookeeperClient::~ZookeeperClient()
{
    int rc = zookeeper_close(zh_);
    if (rc != ZOK) {
        CLog(WARNING) << "Failed to close zookeeper client.  error: " << zerror(rc);
    }
}

void ZookeeperClient::init()
{
    int tryCnt = 0;
    zh_ = zookeeper_init(servers_.c_str(), &ZookeeperClient::watcherFn, 30000, 0, this, 0);
    if (zh_ == nullptr) {
        CLog(FATAL) << "Failed to initialize zookeeper instance";
        throw ZookeeperException(ZINVALIDSTATE );
    }

    while (zoo_state(zh_) != ZOO_CONNECTED_STATE && tryCnt < MAX_CONN_TRIES) {
        tryCnt++;
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    if (zoo_state(zh_) != ZOO_CONNECTED_STATE) {
        CLog(ERROR) << "Failed to connect to zookeeper servers";
        throw ZookeeperException(ZINVALIDSTATE );
    }
}

void ZookeeperClient::close()
{
    zookeeper_close(zh_);
}

void ZookeeperClient::watcher(int type, int state, const char *path)
{
    CLog(INFO) << "type:" << typeToStr(type)
        << " state:" << stateToStr(state)
        << " path:" << path;
}

std::string ZookeeperClient::typeToStr(int type)
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

std::string ZookeeperClient::stateToStr(int state)
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

folly::Future<std::string> ZookeeperClient::put(const std::string &key,
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

folly::Future<std::string> ZookeeperClient::get(const std::string &key)
{
    StringPromise *promise = new StringPromise();
    auto future = promise->getFuture();
    auto rc = zoo_awget(zh_, key.c_str(), nullptr, nullptr,
                        &dataCompletionCb, promise);
    if (rc != ZOK) {
        CLog(WARNING) << "failed to get key: " << key << " error: " << zerror(rc);
        delete promise;
        return folly::makeFuture<std::string>(ZookeeperException(rc));
    }
    return future;
}

}  // namespace cluster
