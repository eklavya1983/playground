#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <folly/futures/Future.h>
#include <folly/io/async/EventBase.h>
#include <actor/ActorMsg.h>

namespace actor {

struct Actor;
struct ActorSystem;
struct RequestTracker;

struct RequestIf {
    RequestIf();
    virtual ~RequestIf() { }
    virtual folly::Future<void> fire() = 0;
    virtual void handleResponse(ActorMsg &&msg) = 0;
    void setId(const RequestId& id);
    RequestId getId() const;
    void setTracker(RequestTracker *tracker);

 protected:
    RequestId       id_;
    ActorId         from_;
    int32_t         timeoutMs_;
    ActorMsgTypeId  payloadTypeId_; 
    Payload         payload_;
    RequestTracker  *tracker_;
};
template<typename T>
using RequestPtr = std::shared_ptr<T>;

template<class T>
struct RequestT : RequestIf {
    using CompletionCb = std::function<void(const Error&, T&)>;

    T& withId(const RequestId &id);
    T& from(const ActorId &id);
    T& withTimeout(int32_t timeoutMs);
    T& withCompletionCb(const CompletionCb &cb);
    template<class PayloadT>
    T& withPayload(const PayloadPtr<PayloadT>&payload);

 protected:
    folly::Promise<void>                    promise_;
    CompletionCb                            completionCb_;
};

template<class T>
T& RequestT<T>::withId(const RequestId &id) {
    id_ = id;
    return static_cast<T&>(*this);
}

template<class T>
T& RequestT<T>::from(const ActorId &id) {
    from_ = id;
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
template<class PayloadT>
T& RequestT<T>::withPayload(const PayloadPtr<PayloadT>&payload) {
    payloadTypeId_ = ActorMsgTypeEnum<PayloadT>::typeId;
    payload_ = payload;
    return static_cast<T&>(*this);
}

struct ActorRequest : RequestT<ActorRequest>{
    ActorRequest& toActor(const ActorId &id);
    virtual folly::Future<void> fire() override;
    virtual void handleResponse(ActorMsg &&msg) override;

    template <class T>
    inline T& respPayload() {
        return respMsg_.payload<T>();
    }
    template <class T>
    inline const T& respPayload() const {
        return respMsg_.payload<T>();
    }
    template <class T>
    inline PayloadPtr<T> respPayloadBuffer() const {
        return respMsg_.buffer<T>();
    }
 protected:
    ActorId             to_;
    ActorMsg            respMsg_;
};

struct QuorumRequest : RequestT<QuorumRequest> {
    QuorumRequest();
    virtual folly::Future<void> fire() override;
    virtual void handleResponse(ActorMsg &&msg) override;
    QuorumRequest& withQuorum(int32_t quorumCnt);
    QuorumRequest& toActors(const std::vector<ActorId> &ids);
    inline const int32_t& ackSuccessCnt() const { return ackSuccessCnt_; }
    inline const int32_t& ackFailedCnt() const { return ackFailedCnt_; }

 protected:
    int32_t                     quorumCnt_;
    std::vector<ActorId>        actorIds_;
    int32_t                     ackSuccessCnt_;
    int32_t                     ackFailedCnt_;
};

/**
* @brief Request tracker tracks requests.
* All operations for RequestTracker must be executed on an event base
*/
struct RequestTracker {
    explicit RequestTracker(ActorSystem *system, folly::EventBase *eb);
    virtual ~RequestTracker() { }
    template<class T, class ... ArgsT>
    RequestPtr<T> allocRequest(ArgsT &&... args);
    void addRequest(RequestPtr<RequestIf> &&req);
    void removeRequest(const RequestId &id);
    void handleResponse(ActorMsg &&msg);
    inline ActorSystem* system() { return system_; }

 protected:
    RequestId incrRequestId_();
    std::unordered_map<RequestId, RequestPtr<RequestIf>>    table_;
    RequestId               nextRequestId_;
    ActorSystem             *system_;
    folly::EventBase        *eb_;
};

template<class T, class ... ArgsT>
RequestPtr<T> RequestTracker::allocRequest(ArgsT &&... args) {
    DCHECK(eb_->isInEventBaseThread());
    RequestPtr<T> ptr(new T(std::forward<ArgsT>(args)...));
    ptr->setTracker(this);
    addRequest(ptr);
    return ptr;
}

}  // namespace actor
