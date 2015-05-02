#pragma once

#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {
#define MSGDIRECTION_NORMAL         0
#define MSGDIRECTION_RESPONSE       1

using namespace cpp2;

using Payload = std::shared_ptr<void>;
// using ActorMsg = std::pair<ActorMsgHeader, Payload>;

struct ActorMsg {
    ActorMsg();
    ActorMsg(const ActorMsgHeader &hdr, const Payload &buf);
    ActorMsg(const ActorMsg &msg);
    ActorMsg(ActorMsg &&msg);
    ~ActorMsg();

    ActorMsg& operator=(const ActorMsg&) = default;
    ActorMsg& operator=(ActorMsg&&) = default;

    inline const int8_t& direction() const { return hdr.direction; }
    inline ActorMsg& direction(const int8_t &dir) {
        hdr.direction = dir;
        return *this;
    }

    inline const ActorMsgTypeId& typeId() const { return hdr.typeId; }
    inline ActorMsg& typeId(const ActorMsgTypeId& id) {
        hdr.typeId = id;
        return *this;
    }

    inline const ActorId& from() const { return hdr.from; }
    inline ActorMsg& from(const ActorId &id) {
        hdr.from = id;
        return *this;
    }

    inline const ActorId& to() const { return hdr.to; }
    inline ActorMsg& to(const ActorId &id) {
        hdr.to = id;
        return *this;
    }

    inline const RequestId& requestId() const { return hdr.requestId; }
    inline ActorMsg& requestId(const RequestId &id) {
        hdr.requestId = id;
        return *this;
    }
    inline bool isTracked() const { return requestId() != ActorMsg::UNTRACKED_ID; }

    const ActorMsgHeader& header() const { return hdr; }

    template <class T>
    inline T& payload() {
        return *(reinterpret_cast<T*>(buf.get()));
    }
    template <class T>
    inline const T& payload() const {
        return *(reinterpret_cast<T*>(buf.get()));
    }
    inline ActorMsg& payload(const Payload &buf) {
        this->buf = buf;
        return *this;
    }

    const static ActorMsgTypeId INVALID_MSGTYPEID = 0;
    const static RequestId UNTRACKED_ID = 0;

    ActorMsgHeader hdr;
    Payload buf;
};

template <class T>
inline T& operator << (T &stream, const ActorMsg& msg) {
    stream << msg.hdr;
    return stream;
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolWriter>
inline void serializeActorMsg(const MsgT &msg, std::unique_ptr<folly::IOBuf> &serializedBuf) {
    ProtocolT writer;
    folly::IOBufQueue queue(folly::IOBufQueue::cacheChainLength());
    size_t bufSize = msg.serializedSizeZC(&writer);
    writer.setOutput(&queue, bufSize);
    msg.write(&writer);
    serializedBuf = queue.move();
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
inline std::shared_ptr<MsgT> deserializeActorMsg(const folly::IOBuf *buf) {
    std::shared_ptr<MsgT> deserializedMsg = std::make_shared<MsgT>();
    deserializeActorMsg<MsgT, ProtocolT>(buf, *deserializedMsg);
    return deserializedMsg;
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
inline void deserializeActorMsg(const folly::IOBuf *buf, MsgT &deserializedMsg) {
    ProtocolT reader;
    reader.setInput(buf);
    deserializedMsg.read(&reader);
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolWriter>
inline void toIOBuf(const ActorMsg &msg, std::unique_ptr<folly::IOBuf> &buf) {
    serializeActorMsg<MsgT, ProtocolT>(msg.payload<MsgT>(), buf);
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
inline void toActorMsg(const std::unique_ptr<folly::IOBuf> &buf, ActorMsg &msg) {
    std::shared_ptr<MsgT> deserializedMsg = std::make_shared<MsgT>();
    deserializeActorMsg<MsgT, ProtocolT>(buf.get(), *deserializedMsg);
    msg.payload(deserializedMsg);
}

#define ADD_MSGMAPPING(type, id) addActorMsgTypeMapping<type, id>(#type)

/* To store the type id for type T*/
template <class MsgT>
struct ActorMsgTypeEnum {
    static ActorMsgTypeId       typeId;
};
template <ActorMsgTypeId id>
struct ActorMsgTypeName {
    static const char* typeName;
};
template<class T>
ActorMsgTypeId ActorMsgTypeEnum<T>::typeId = ActorMsg::INVALID_MSGTYPEID;
template<ActorMsgTypeId id>
const char* ActorMsgTypeName<id>::typeName;

using SerializerF = std::function<void (const ActorMsg&, std::unique_ptr<folly::IOBuf>&)>;
using DeserializerF = std::function<void (const std::unique_ptr<folly::IOBuf>&, ActorMsg&)>;
struct  ActorMsgTypeInfo {
    const char*          typeName;
    SerializerF          serializer;
    DeserializerF        deserializer;
};
using SerializerTbl = std::unordered_map<ActorMsgTypeId, ActorMsgTypeInfo>;
extern SerializerTbl *gMsgTypeInfoTbl;

template <class MsgT, ActorMsgTypeId typeId>
void addActorMsgTypeMapping(const char* typeName) {
    /* Store enum id in ActorMsgTypeEnum */
    ActorMsgTypeEnum<MsgT>::typeId = typeId;
    ActorMsgTypeName<typeId>::typeName= typeName;

    /* Map the serializers and deserializer for the the type */
    if (gMsgTypeInfoTbl == nullptr) {
        gMsgTypeInfoTbl = new SerializerTbl();
    }
    CHECK(gMsgTypeInfoTbl->find(typeId) == gMsgTypeInfoTbl->end())
        << typeId << " is already registered";
    (*gMsgTypeInfoTbl)[typeId] = {typeName, &toIOBuf<MsgT>, &toActorMsg<MsgT>};
}
void initCommonActorMsgMappings();
void clearActorMappings();
const char* actorMsgName(ActorMsgTypeId id);


/**
* @brief 
*
* @tparam MsgT
* @param direction
* @param from
* @param to
* @param requestId
* @param payload
*
* @return 
*/
template <class MsgT>
inline ActorMsg makeActorMsgCommon(const int8_t &direction,
                                   const ActorId &from,
                                   const ActorId &to,
                                   const RequestId &requestId,
                                   std::shared_ptr<void> &&payload) {
    ActorMsg msg;
    msg.direction(direction)
        .typeId(ActorMsgTypeEnum<MsgT>::typeId)
        .from(from)
        .to(to)
        .requestId(requestId)
        .payload(payload);
    return msg;
}
/**
* @brief Use this function for createing an actor message
*
* @tparam MsgT
* @param from
* @param to
* @param payload
*
* @return 
*/
template <class MsgT>
inline ActorMsg makeActorMsg(const ActorId &from, const ActorId &to,
                             std::shared_ptr<void> &&payload) {
    return makeActorMsgCommon<MsgT>(MSGDIRECTION_NORMAL,
                                    from, to,
                                    ActorMsg::UNTRACKED_ID,
                                    std::move(payload));
}

template <class MsgT>
inline ActorMsg makeActorMsg() {
    return makeActorMsg<MsgT>(ActorId(), ActorId(), nullptr);
}
}  // namesapce actor

