#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <actor/ActorMsg.h>

#define on(msgType) OnMsg(ACTORMSGTYPEID(msgType))

namespace actor {

using namespace cpp2;

using BehaviorHandler = std::function<void (ActorMsg&&)>;

struct BehaviorItem {
    ActorMsgTypeId typeId;
    BehaviorHandler handler;
};

// TODO: Check if this can be made more efficient in terms of reducing copies
struct OnMsg {
    OnMsg(const ActorMsgTypeId& typeId) {
        this->typeId = typeId;
    }
    BehaviorItem operator>>(BehaviorHandler handler) {
        return {typeId, handler};
    }
    ActorMsgTypeId typeId;
};

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
            handlers_[item.typeId] = item.handler;
        }
    }

    inline void handle(ActorMsg &&msg) {
        auto handlerItr = handlers_.find(actorMsgTypeId(msg));
        if (handlerItr == handlers_.end()) {
            handlerItr = handlers_.find(static_cast<ActorMsgTypeId>(ActorMsgTypeIds::OtherMsg));
            if (handlerItr == handlers_.end()) {
                assert(!"TODO: Implement"); 
                // TODO: Determine behavior here
                return;
            }
        }
        (handlerItr->second)(std::move(msg));
    }
 protected:
    std::unordered_map<ActorMsgTypeId, BehaviorHandler> handlers_;
};
}  // namespace actor
