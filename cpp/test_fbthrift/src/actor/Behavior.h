#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <actor/ActorMsg.h>

namespace actor {

using namespace cpp2;

using BehaviorHandler = std::function<void (ActorMsg&&)>;

struct BehaviorItem {
    ActorMsgType type;
    BehaviorHandler handler;
};

// TODO: Check if this can be made more efficient in terms of reducing copies
struct On {
    On(const ActorMsgType& type) {
        this->type = type;
    }
    BehaviorItem operator>>(BehaviorHandler handler) {
        return {type, handler};
    }
    ActorMsgType type;
};

template <class MsgEnumT>
On on(const MsgEnumT& type) {
    return On(static_cast<ActorMsgType>(type));
}

struct Behavior {
    Behavior() {}
    Behavior(const std::vector<BehaviorItem>& items) {
        this->operator=(items);
    }

    void operator = (const std::vector<BehaviorItem>& items)
    {
        handlers_.clear();
        this->operator+=(items);
    }

    void operator += (const std::vector<BehaviorItem>& items)
    {
        for (auto &item : items) {
            handlers_[item.type] = item.handler;
        }
    }

    inline void handle(ActorMsg &&msg) {
        auto handlerItr = handlers_.find(actorMsgType(msg));
        if (handlerItr == handlers_.end()) {
            handlerItr = handlers_.find(static_cast<ActorMsgType>(ActorMsgTypes::OtherMsg));
            if (handlerItr == handlers_.end()) {
                assert(!"TODO: Implement"); 
                // TODO: Determine behavior here
                return;
            }
        }
        (handlerItr->second)(std::move(msg));
    }
 protected:
    std::unordered_map<ActorMsgType, BehaviorHandler> handlers_;
};
}  // namespace actor
