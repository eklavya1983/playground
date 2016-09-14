#pragma once

#include <zookeeper.h>
#include <string>
#include <stdexcept>

namespace cluster {

/**
 * @brief Connection exception
 */
struct ZookeeperConnectionException : std::runtime_error {
    ZookeeperConnectionException()
        : std::runtime_error("Failed to connect to zookeeper server")
    {
    }
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

    ZookeeperClient(const std::string& servers);
    void init();
    void close();
    void watcher(int type, int state, const char *path);

    std::string typeToStr(int type);
    std::string stateToStr(int state);

    zhandle_t* getHandle() { return zh_; }

 private:
    /* List of zookeeper servers */
    std::string         servers_;
    /* Zookeeper handle */
    zhandle_t           *zh_ {nullptr};
};

}  // namespace cluster
