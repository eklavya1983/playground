#include <util/Log.h>
#include <actor/ActorUtil.h>
#include <actor/ActorRequest.h>

namespace actor {

RequestIf::RequestIf() {
    id_ = ActorMsg::UNTRACKED_ID;
    timeoutMs_ = 0;
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

QuorumRequest::QuorumRequest()
: ackCnt_(0) {
}

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
    ackCnt_++;
    if (ackCnt_ == quorumCnt_) {
        completionCb_(Error::ERR_OK, *this);        
        tracker_->removeRequest(id_);
    }
}

RequestTracker::RequestTracker()
 : nextRequestId_(ActorMsg::UNTRACKED_ID + 1) {
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

void RequestTracker::onResponse(ActorMsg &&msg) {
    auto itr = table_.find(msg.requestId());
    if (itr == table_.end()) {
        LOG(WARNING) << "Request not found. Dropping message: " << msg;
        return;
    }
    itr->second->onResponse(std::move(msg));
}

}  // namespace actor
