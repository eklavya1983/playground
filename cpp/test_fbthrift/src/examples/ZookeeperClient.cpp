#include <ZookeeperClient.h>
#include <chrono>
#include <thread>
#include <glog/logging.h>

namespace cluster {

void ZookeeperClient::watcherFn(zhandle_t *zh,
                                int type,
                                int state,
                                const char *path,
                                void *watcherCtx)
{
    static_cast<ZookeeperClient*>(watcherCtx)->watcher(type, state, path);
}

ZookeeperClient::ZookeeperClient(const std::string& servers)
    : servers_(servers)
{
}

void ZookeeperClient::init()
{
    int tryCnt = 0;
    while (tryCnt < MAX_CONN_TRIES) {
        zh_ = zookeeper_init(servers_.c_str(), &ZookeeperClient::watcherFn, 30000, 0, this, 0);
        if (zh_ != nullptr) {
            LOG(INFO) << "connected to zookeeper server.  establishing session...";
            break;
        }

        if (tryCnt < MAX_CONN_TRIES) {
            LOG(WARNING) << "failed to connect; will try again. attempt:" << tryCnt;
            ++tryCnt;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        } else {
            throw ZookeeperConnectionException();
        }
    }
}

void ZookeeperClient::close()
{
    zookeeper_close(zh_);
}

void ZookeeperClient::watcher(int type, int state, const char *path)
{
    LOG(INFO) << "type:" << typeToStr(type)
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

}  // namespace cluster
