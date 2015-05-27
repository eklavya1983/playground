#include <util/Log.h>
#include <actor/ActorUtil.h>
#include <actor/ActorSystem.hpp>
#include <actor/ActorRequest.h>

namespace actor {

RequestIf::RequestIf() {
    id_ = ActorMsg::UNTRACKED_ID;
    from_ = ActorSystem::invalidActorId();
    timeoutMs_ = 0;
    payloadTypeId_ = ActorMsg::INVALID_MSGTYPEID;
    payload_ = nullptr;
    tracker_ = nullptr;
}

void RequestIf::setId(const RequestId& id) {
    id_ = id;
}

RequestId RequestIf::getId() const {
    return id_;
}

void RequestIf::setTracker(RequestTracker *tracker) {
    tracker_ = tracker;
}

ActorRequest& ActorRequest::toActor(const ActorId &id) {
    to_ = id;
    return *this;
}

folly::Future<void> ActorRequest::fire() {
    DCHECK(from_ != ActorSystem::invalidActorId());
    DCHECK(to_ != ActorSystem::invalidActorId());

    ActorMsg msg;
    msg.typeId(payloadTypeId_)
        .from(from_)
        .to(to_)
        .requestId(id_)
        .payload(payload_);

    auto system_ = tracker_->system();
    ROUTE(std::move(msg));

    return promise_.getFuture();
}

void ActorRequest::handleResponse(ActorMsg &&msg) {
    // TODO: Handle
    // 1. Errors
    // 2. Same resonse
    respMsg_ = std::move(msg);
    promise_.setValue();
    tracker_->removeRequest(id_);
}

QuorumRequest::QuorumRequest()
: ackSuccessCnt_(0), ackFailedCnt_(0)
{
}

QuorumRequest& QuorumRequest::withQuorum(int32_t quorumCnt) {
    quorumCnt_ = quorumCnt; 
    return *this;
}

QuorumRequest& QuorumRequest::toActors(const std::vector<ActorId> &ids) {
    actorIds_ = ids;
    return *this;
}

folly::Future<void> QuorumRequest::fire() {
    return promise_.getFuture();
}

void QuorumRequest::handleResponse(ActorMsg &&msg) {
    // TODO: look at the error 
    ackSuccessCnt_++;
    if ((ackSuccessCnt_ + ackFailedCnt_) == quorumCnt_) {
        // completionCb_(Error::ERR_OK, *this);        
        tracker_->removeRequest(id_);
    }
    // todo: set the promise
}

RequestTracker::RequestTracker(ActorSystem *system, folly::EventBase *eb)
 : nextRequestId_(ActorMsg::UNTRACKED_ID + 1) {
    system_ = system;
    eb_ = eb;
}

RequestId RequestTracker::incrRequestId_() {
    return nextRequestId_++;
}

void RequestTracker::addRequest(RequestPtr<RequestIf> &&req) {
    DCHECK(eb_->isInEventBaseThread());
    auto id = incrRequestId_();
    req->setId(id);
    table_[id] = std::move(req);
}

void RequestTracker::removeRequest(const RequestId &id) {
    DCHECK(eb_->isInEventBaseThread());
    size_t ret = table_.erase(id);
    DCHECK(ret == 1);
}

void RequestTracker::handleResponse(ActorMsg &&msg) {
    DCHECK(eb_->isInEventBaseThread());
    auto itr = table_.find(msg.requestId());
    if (itr == table_.end()) {
        LOG(WARNING) << "Request not found. Dropping message: " << msg;
        DCHECK(false);
        return;
    }
    itr->second->handleResponse(std::move(msg));
}

}  // namespace actor
