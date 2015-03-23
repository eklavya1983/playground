#pragma once

#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

using namespace cpp2;

using Payload = std::shared_ptr<void>;
using ActorMsg = std::pair<ActorMsgHeader, Payload>;

inline ActorMsgTypeId actorMsgTypeId(const ActorMsg &msg) {
    return msg.first.typeId;
}
inline void setActorMsgTypeId(ActorMsg &msg, ActorMsgTypeId typeId) {
    msg.first.typeId = typeId;
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
    serializeActorMsg<MsgT, ProtocolT>(*(std::static_pointer_cast<MsgT>(msg.second)), buf);
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
inline void toActorMsg(const std::unique_ptr<folly::IOBuf> &buf, ActorMsg &msg) {
    std::shared_ptr<MsgT> deserializedMsg = std::make_shared<MsgT>();
    deserializeActorMsg<MsgT, ProtocolT>(buf.get(), *deserializedMsg);
    msg.second = deserializedMsg;
}

/* Typedefs */
using SerializerF = std::function<void (const ActorMsg&, std::unique_ptr<folly::IOBuf>&)>;
using DeserializerF = std::function<void (const std::unique_ptr<folly::IOBuf>&, ActorMsg&)>;
using SerializerTbl = std::unordered_map<ActorMsgTypeId, std::pair<SerializerF, DeserializerF>>;
extern SerializerTbl *gMsgMapTbl;

/* To store the type id for type T*/
template <class T>
struct ActorMsgTypeInfo {
    static ActorMsgTypeId typeId;
};

template <class MsgT, class EnumT>
void addActorMsgMapping(EnumT e) {
    /* Store enum id in ActorMsgTypeInfo */
    auto typeId = static_cast<ActorMsgTypeId>(e);
    ActorMsgTypeInfo<MsgT>::typeId = typeId;

    if (e <= ActorMsgTypeIds::NOPAYLOAD_MSG_END) {
        /* Don't add serializers for types that don't necessarily have payloads */
        return;
    }

    /* Map the serializers and deserializer for the the type */
    if (gMsgMapTbl == nullptr) {
        gMsgMapTbl = new SerializerTbl();
    }
    CHECK(gMsgMapTbl->find(typeId) == gMsgMapTbl->end())
        << typeId << " is already registered";
    (*gMsgMapTbl)[typeId] = {&toIOBuf<MsgT>, &toActorMsg<MsgT>};
}
extern void initActorMsgMappings();


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
    ActorMsg msg;
    auto &header = msg.first;
    header.typeId = ActorMsgTypeInfo<MsgT>::typeId;
    header.from = from;
    header.to = to;
    msg.second = std::move(payload);
    return msg;
}

template <class MsgT>
inline ActorMsg makeActorMsg() {
    return makeActorMsg<MsgT>(ActorId(), ActorId(), nullptr);
}
}  // namesapce actor

