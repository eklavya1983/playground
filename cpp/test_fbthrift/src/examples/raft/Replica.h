#pragma once
#include <actor/Actor.h>


namespace raft {

using namespace actor;
using namespace actor::cpp2;
using Term = int32_t;

using EntryOpType = int8_t;
struct Entry {
   EntryOpType      type; 
   std::string      key;
   std::string      val;
};

struct AppendEntries : LocalPayload<> {
    Term            term;
    ActorId         leaderId;
    uint64_t        prevLogIdx;
    Term            prevLogTerm;
    std::vector<Entry>  entries;
    uint64_t        leaderCommitIdx;
};

struct AppendEntriesResp : LocalPayload<> {
    int             error;
    Term            term;
    uint64_t        followerCommitIdx;
};

struct RequestVote : LocalPayload<> {
    Term            term;
    ActorId         candidateId;
    uint64_t        lastLogIdx;
    Term            lastLogTerm;
};

struct RequestVoteResp : LocalPayload<> {
    Term            term;
    bool            success;
};

struct TransToCandidateCheck : LocalPayload<> {
};

struct IoReq : LocalPayload<> {
};

enum {
    // TODO: Set begin and end for enums
    LOG_ENTRY_NOTFOUND,
    STALE_TERM
};

template<class LogT, class StatemachineT>
struct Replica : NotificationQueueActor {
    Replica(const int nReplicas,
            std::unique_ptr<LogT> opLog,
            std::unique_ptr<StatemachineT> store); 
    void changeBehavior(Behavior *toBehavior);

 protected:
    void initBehaviors_();
    void scheduleTransToCandidateCheck_();
    void sendRequestForVotes_();
    inline bool isFollower_() { return currentBehavior_ == &follower_; }
    inline bool isCandidate_() { return currentBehavior_ == &candidate_; }
    inline bool isLeader_() { return currentBehavior_ == &leader_; }

    Behavior                            follower_;
    Behavior                            candidate_;
    Behavior                            leader_;

    int                                 nReplicas_;

    /* Persistent state on everyone */
    std::unique_ptr<LogT>               opLog_;
    std::unique_ptr<StatemachineT>      store_;
    Term                                currentTerm_;
    ActorId                             votedFor_;

    /* Volatile state on everyone */
    uint64_t                            commitIdx_;
    uint64_t                            lastApplied_;

    /* Volatile state on leader */
    std::vector<uint64_t>               nextIdx_;
    std::vector<uint64_t>               matchIdx_;

    std::chrono::time_point<std::chrono::steady_clock>  lastAppendEntryTime_;
    const static std::chrono::milliseconds ELECTION_TIMEOUT_MS                  =  2000ms;
    const static std::chrono::milliseconds TRANS_TO_CANDIDATE_CHECK_INTERVAL_MS =  500ms;
};

template<class LogT, class StatemachineT>
Replica<LogT, StatemachineT>::Replica(const int nReplicas,
                                      std::unique_ptr<LogT> opLog,
                                      std::unique_ptr<StatemachineT> store) {
    nReplicas_ = nReplicas;
    opLog_ = std::move(opLog);
    store_ = std::move(store);
}

template<class LogT, class StatemachineT>
void Replica<LogT, StatemachineT>::initBehaviors_() {
#define APPEND_TERM_CHECK() \
    do { \
    if (payload<AppendEntriesResp>().term < currentTerm_) { \
        respPayload->term = currentTerm_; \
        respPayload->error = STALE_TERM; \
        REPLY(AppendEntriesResp, std::move(respPayload)); \
    } \
    } while (false)

    follower_ += {
        on(AppendEntries) >> [this]() {
            auto &appendReq = payload<AppendEntries>();
            auto respPayload = std::make_shared<AppendEntriesResp>();
            lastAppendEntryTime_ = std::chrono::steady_clock::now();

            CHECK(appendReq.prevLogIdx > commitIdx_);

            APPEND_TERM_CHECK();

            bool appended = opLog_->findAndAppend(appendReq.prevLogIdx,
                                                 appendReq.prevLogTerm,
                                                 appendReq.entries);
            if (!appended) {
               respPayload->error = LOG_ENTRY_NOTFOUND;
               respPayload->followerCommitIdx =  commitIdx_;
            }
            opLog_->updateCommitIdx(appendReq.leaderCommitIdx);
            REPLY(AppendEntriesResp, std::move(respPayload));
        },
        on(RequestVote) >> [this]() {
        },
        on(TransToCandidateCheck) >> [this]() {
            auto curTime = std::chrono::steady_clock::now();
            if (curTime - lastAppendEntryTime_ > ELECTION_TIMEOUT_MS) {
                changeBehavior(&candidate_);
            } else {
                scheduleTransToCandidateCheck_();
            }
        },
        on(IoReq) >> [this]() {
            // TODO: Reply to client with right leader
        }
    };

    candidate_ += {
        on(AppendEntries) >> [this]() {
        },
        on(RequestVote) >> [this]() {
        },
        on(IoReq) >> [this]() {
            // TODO: Reply to client to retry after sometime
        }
    };

    leader_ += {
        on(AppendEntries) >> [this]() {
        },
        on(RequestVote) >> [this]() {
        },
        on(IoReq) >> [this]() {
        }
    };
}

template<class LogT, class StatemachineT>
void Replica<LogT, StatemachineT>::changeBehavior(Behavior *toBehavior) {
    if (toBehavior == &follower_) {
        currentBehavior_ = toBehavior;
        scheduleTransToCandidateCheck_();
    } else if(toBehavior == &candidate_) {
        currentBehavior_ = toBehavior;
        sendRequestForVotes_();
    }
}

template<class LogT, class StatemachineT>
void Replica<LogT, StatemachineT>::scheduleTransToCandidateCheck_() {
    eventBase_->runAfterDelay(
        [this]() {
            if (isFollower_()) {
                handle(makeActorMsg<TransToCandidateCheck>());
            }
        },
        TRANS_TO_CANDIDATE_CHECK_INTERVAL_MS);
}

template<class LogT, class StatemachineT>
void Replica<LogT, StatemachineT>::sendRequestForVotes_() {
}

}  // namepsace raft
