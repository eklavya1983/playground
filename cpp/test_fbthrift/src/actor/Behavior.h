#pragma once

#include <vector>
#include <functional>
#include <unordered_map>
#include <folly/Demangle.h>
#include <actor/ActorMsg.h>

#define on(msgT)        OnMsg({MSGDIRECTION_NORMAL, ActorMsgTypeEnum<msgT>::typeId}, className())
#define onresp(msgT)    OnMsg({MSGDIRECTION_RESPONSE, ActorMsgTypeEnum<msgT>::typeId}, className())

namespace actor {

using namespace cpp2;

using BehaviorHandler = std::function<void ()>;

struct BehaviorKey {
    BehaviorKey() {
        this->direction = MSGDIRECTION_NORMAL;
        this->typeId = ActorMsg::INVALID_MSGTYPEID;
    }
    BehaviorKey(const int8_t &direction, const ActorMsgTypeId &typeId) {
        this->direction = direction;
        this->typeId = typeId;
    }
    BehaviorKey(const BehaviorKey& b) = default;
    BehaviorKey& operator=(const BehaviorKey& b) = default;
    bool operator==(const BehaviorKey &k) const {
        return direction == k.direction && typeId == k.typeId;
    }

    int8_t direction;
    ActorMsgTypeId typeId;
};
std::ostream& operator<<(std::ostream &stream, const BehaviorKey &b);

struct BehaviorKeyHash {
    static_assert(sizeof(ActorMsgTypeId) <= sizeof(int32_t), "ActorMsgTypeId size > 4 bytes");
    std::size_t operator()(const BehaviorKey& k) const
    {
        using std::size_t;
        return (static_cast<size_t>(k.direction) << 32) | static_cast<size_t>(k.typeId);
    }
};

struct BehaviorItem {
    BehaviorKey behaviorKey;
    BehaviorHandler handler;
    /* Added for debugging */
    std::string handlerName;

    friend std::ostream& operator<<(std::ostream &stream, const BehaviorItem &b);
};
std::ostream& operator<<(std::ostream &stream, const BehaviorItem &b);

// TODO: Check if this can be made more efficient in terms of reducing copies
struct OnMsg {
    OnMsg(const ActorMsgTypeId &typeId)
    : OnMsg(BehaviorKey(MSGDIRECTION_NORMAL, typeId))
    {
    }
    OnMsg(const BehaviorKey &behaviorKey)
    : OnMsg(behaviorKey, "") {
    }
    OnMsg(const BehaviorKey &behaviorKey, const std::string &handlerName) {
        CHECK(behaviorKey.typeId != ActorMsg::INVALID_MSGTYPEID) << "Trying to use an unmapped type";
        this->behaviorKey = behaviorKey;
        this->handlerName = handlerName;
    }
    BehaviorItem operator>>(BehaviorHandler handler) {
        return {behaviorKey, handler, handlerName};
    }

    BehaviorKey behaviorKey;
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
            CHECK(item.behaviorKey.typeId != ActorMsg::INVALID_MSGTYPEID);
            CHECK(item.behaviorKey.direction == MSGDIRECTION_NORMAL ||
                  item.behaviorKey.direction == MSGDIRECTION_RESPONSE);
            handlers_[item.behaviorKey] = item;
            /* adjust handler name */
            handlers_[item.behaviorKey].handlerName = item.handlerName + std::string("_") + name();
        }
    }

    void handle(const BehaviorKey& key);

    const std::string& name() const { return name_; }

    friend std::ostream& operator<<(std::ostream& stream, const Behavior& b);
 protected:
    std::string name_;
    std::unordered_map<BehaviorKey, BehaviorItem, BehaviorKeyHash> handlers_;
};

std::ostream& operator<<(std::ostream& stream, const Behavior& b);
}  // namespace actor
