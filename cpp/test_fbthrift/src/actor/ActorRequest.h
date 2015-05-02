#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <actor/ActorMsg.h>

namespace actor {

struct RequestIf {
    virtual ~RequestIf() { }
    virtual void fire() = 0;
    virtual void onResponse(ActorMsg &&msg) = 0;
    void setId(const RequestId& id);
    RequestId getId() const;

 protected:
    RequestId       id_;
    int32_t         timeoutMs_;
    Payload         payload_;
};
using RequestPtr = std::unique_ptr<RequestIf>;

void RequestIf::setId(const RequestId& id) {
    id_ = id;
}
RequestId RequestIf::getId() const {
    return id_;
}

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
    virtual void fire() override;
    virtual void onResponse(ActorMsg &&msg) override;
    QuorumRequest& withQuorum(int32_t quorumCnt);
    QuorumRequest& toActors(std::vector<ActorId> &&ids);

 protected:
    int32_t quorumCnt_;
    std::vector<ActorId> actorIds_;
};

QuorumRequest& QuorumRequest::withQuorum(int32_t quorumCnt) {
    quorumCnt_ = quorumCnt; 
    return *this;
}
QuorumRequest& QuorumRequest::toActors(std::vector<ActorId> &&ids) {
    actorIds_ = std::move(ids);
    return *this;
}

void QuorumRequest::fire() {
}

void QuorumRequest::onResponse(ActorMsg &&msg) {
}

struct RequestTracker {
    RequestTracker();

    virtual ~RequestTracker() { }

    void addRequest(RequestPtr &&req);

    void removeRequest(const RequestId &id);

    template<class T, class ... ArgsT>
    T& allocRequest(ArgsT &&... args);

    static const RequestId UNTRACKED_ID = 0;
 protected:
    RequestId incrRequestId_();
    std::unordered_map<RequestId, RequestPtr> table_;
    RequestId nextRequestId_;
};

RequestTracker::RequestTracker()
 : nextRequestId_(UNTRACKED_ID + 1) {
}

RequestId RequestTracker::incrRequestId_() {
    return nextRequestId_++;
}

void RequestTracker::addRequest(RequestPtr &&req) {
    auto id = incrRequestId_();
    req->setId(id);
    table_[incrRequestId_()] = std::move(req);
}

void RequestTracker::removeRequest(const RequestId &id) {
    table_.erase(id);
}

template<class T, class ... ArgsT>
T& RequestTracker::allocRequest(ArgsT &&... args) {
    std::unique_ptr<T> ptr(new T(std::forward<ArgsT>(args)...));
    T &ref = *ptr;
    addRequest(std::move(ptr));
    return ref;
}

}  // namespace actor
