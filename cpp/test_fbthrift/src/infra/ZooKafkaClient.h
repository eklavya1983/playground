#pragma once

#include <string>
#include <stdexcept>
#include <infra/CoordinationClient.h>

extern "C" {
typedef struct _zhandle zhandle_t;
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
 * @brief C++ zookeeper kafka client
 */
struct ZooKafkaClient : CoordinationClient {
    /* Maximum connection retries against zookeeper servers */
    const static int MAX_CONN_TRIES = 1;
    static void watcherFn(zhandle_t *zh,
                          int type,
                          int state,
                          const char *path,
                          void *watcherCtx);
    ZooKafkaClient(const std::string &logContext,
                   const std::string& servers);
    ~ZooKafkaClient();
    void init() override;
    void close();

    folly::Future<VersionedData> get(const std::string &key) override;
    folly::Future<std::vector<std::string>> getChildrenSimple(const std::string &key);
#if 0
    folly::Future<std::vector<VersionedData>> getChildren(const std::string &key) override;
#endif
    std::vector<KVPair> getChildrenSync(const std::string &key) override;
    folly::Future<std::string> put(const std::string &key, const std::string &value);

    std::string typeToStr(int type);
    std::string stateToStr(int state);
    void watcher(int type, int state, const char *path);

    zhandle_t* getHandle() { return zh_; }

    inline std::string getLogContext() const { return logContext_; }

 protected:
    void blockUntilConnectedOrTimedOut_(int seconds);

    std::string                                     logContext_;
    /* List of zookeeper servers */
    std::string                                     servers_;
    /* Zookeeper handle */
    zhandle_t                                       *zh_ {nullptr};
};

}  // namespace infra
