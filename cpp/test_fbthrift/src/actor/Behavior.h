#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <folly/Demangle.h>
#include <actor/ActorMsg.h>

#define on(msgT) OnMsg(ActorMsgTypeEnum<msgT>::typeId, className())

namespace actor {

using namespace cpp2;

using BehaviorHandler = std::function<void ()>;

struct BehaviorItem {
    ActorMsgTypeId typeId;
    BehaviorHandler handler;
    /* Added for debugging */
    std::string handlerName;

    friend std::ostream& operator<<(std::ostream &stream, const BehaviorItem &b);
};
std::ostream& operator<<(std::ostream &stream, const BehaviorItem &b);

// TODO: Check if this can be made more efficient in terms of reducing copies
struct OnMsg {
    OnMsg(const ActorMsgTypeId& typeId)
    : OnMsg(typeId, "") {
    }
    OnMsg(const ActorMsgTypeId& typeId, const std::string &handlerName) {
        this->typeId = typeId;
        this->handlerName = handlerName;
    }
    BehaviorItem operator>>(BehaviorHandler handler) {
        return {typeId, handler, handlerName};
    }

    ActorMsgTypeId typeId;
    std::string handlerName;
};

struct Behavior {
    Behavior(const std::string& name) {name_ = name;}
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
            CHECK(item.typeId != ActorMsg::INVALID_MSGTYPEID);
            handlers_[item.typeId] = item;
            /* adjust handler name */
            handlers_[item.typeId].handlerName = item.handlerName + std::string("_") + name();
        }
    }

    void handle(const ActorMsgTypeId &typeId);

    const std::string& name() const { return name_; }

    friend std::ostream& operator<<(std::ostream& stream, const Behavior& b);
 protected:
    std::string name_;
    std::unordered_map<ActorMsgTypeId, BehaviorItem> handlers_;
};

std::ostream& operator<<(std::ostream& stream, const Behavior& b);
}  // namespace actor
