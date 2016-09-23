#pragma once

#include <string>
#include <stdexcept>

extern "C" {
typedef struct _zhandle zhandle_t;
}

namespace folly {
template <class T>
class Future;
struct Unit;
}

namespace infra {

/**
 * @brief Connection exception
 */
struct ZookeeperException : std::exception {
    ZookeeperException(int ecode)
        : zkError_(ecode)
    {
    }
    int getError() const { return zkError_; }
 private:
    int zkError_{0};
};

/**
 * @brief C++ zookeeper client
 */
struct ZookeeperClient {
    /* Maximum connection retries against zookeeper servers */
    const static int MAX_CONN_TRIES = 1;
    static void watcherFn(zhandle_t *zh,
                          int type,
                          int state,
                          const char *path,
                          void *watcherCtx);
    ZookeeperClient(const std::string &logContext,
                    const std::string& servers);
    ~ZookeeperClient();
    void init();
    void close();

    void blockUntilConnectedOrTimedOut(int seconds);

    void watcher(int type, int state, const char *path);

    folly::Future<std::string> put(const std::string &key, const std::string &value);
    folly::Future<std::string> get(const std::string &key);

    std::string typeToStr(int type);
    std::string stateToStr(int state);

    zhandle_t* getHandle() { return zh_; }

    inline std::string getLogContext() const { return logContext_; }

 private:
    std::string                                     logContext_;
    /* List of zookeeper servers */
    std::string                                     servers_;
    /* Zookeeper handle */
    zhandle_t                                       *zh_ {nullptr};
};

}  // namespace infra
