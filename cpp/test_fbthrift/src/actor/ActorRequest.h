#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <actor/ActorMsg.h>

namespace actor {

struct RequestTracker;

struct RequestIf {
    RequestIf();
    virtual ~RequestIf() { }
    virtual void fire() = 0;
    virtual void onResponse(ActorMsg &&msg) = 0;
    void setId(const RequestId& id);
    RequestId getId() const;
    void setTracker(RequestTracker *tracker);

 protected:
    RequestId       id_;
    int32_t         timeoutMs_;
    Payload         payload_;
    RequestTracker  *tracker_;
};
using RequestPtr = std::unique_ptr<RequestIf>;

template<class T>
struct RequestT : RequestIf {
    using CompletionCb = std::function<void(const Error&, T&)>;

    T& withId(const RequestId &id);
    T& withTimeout(int32_t timeoutMs);
    T& withCompletionCb(const CompletionCb &cb);
    T& withPayload(const Payload &payload);

 protected:
    CompletionCb    completionCb_;
};

template<class T>
T& RequestT<T>::withId(const RequestId &id) {
    id_ = id;
    return static_cast<T&>(*this);
}

template<class T>
T& RequestT<T>::withTimeout(int32_t timeoutMs) {
    timeoutMs_ = timeoutMs;
    return static_cast<T&>(*this);
}

template<class T>
T& RequestT<T>::withCompletionCb(RequestT::CompletionCb const &cb) {
    completionCb_ = cb;
    return static_cast<T&>(*this);
}

template<class T>
T& RequestT<T>::withPayload(const Payload &payload) {
    payload_ = payload;
    return static_cast<T&>(*this);
}

struct QuorumRequest : RequestT<QuorumRequest> {
    QuorumRequest();
    virtual void fire() override;
    virtual void onResponse(ActorMsg &&msg) override;
    QuorumRequest& withQuorum(int32_t quorumCnt);
    QuorumRequest& toActors(std::vector<ActorId> &&ids);

 protected:
    int32_t                     quorumCnt_;
    std::vector<ActorId>        actorIds_;
    int32_t                     ackCnt_;
};

struct RequestTracker {
    RequestTracker();

    virtual ~RequestTracker() { }

    void addRequest(RequestPtr &&req);

    void removeRequest(const RequestId &id);

    void onResponse(ActorMsg &&msg);

    template<class T, class ... ArgsT>
    T& allocRequest(ArgsT &&... args);

 protected:
    RequestId incrRequestId_();
    std::unordered_map<RequestId, RequestPtr> table_;
    RequestId nextRequestId_;
};

template<class T, class ... ArgsT>
T& RequestTracker::allocRequest(ArgsT &&... args) {
    std::unique_ptr<T> ptr(new T(std::forward<ArgsT>(args)...));
    T &ref = *ptr;
    ref.setTracker(this);
    addRequest(std::move(ptr));
    return ref;
}

}  // namespace actor
