#pragma once

#include <thrift/lib/cpp2/protocol/BinaryProtocol.h>
#include <actor/gen-cpp2/Service_types.h>

namespace actor {

using namespace cpp2;

using Payload = std::shared_ptr<void>;
using ActorMsg = std::pair<ActorMsgHeader, Payload>;

inline ActorMsgType actorMsgType(const ActorMsg &msg) {
    return msg.first.type;
}
inline void setActorMsgType(ActorMsg &msg, ActorMsgType type) {
    msg.first.type = type;
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolWriter>
void serializeActorMsg(const MsgT &msg, std::unique_ptr<folly::IOBuf> &serializedBuf) {
    ProtocolT writer;
    folly::IOBufQueue queue(folly::IOBufQueue::cacheChainLength());
    size_t bufSize = msg.serializedSizeZC(&writer);
    writer.setOutput(&queue, bufSize);
    msg.write(&writer);
    serializedBuf = queue.move();
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
std::shared_ptr<MsgT> deserializeActorMsg(const folly::IOBuf *buf) {
    std::shared_ptr<MsgT> deserializedMsg = std::make_shared<MsgT>();
    deserializeActorMsg<MsgT, ProtocolT>(buf, *deserializedMsg);
    return deserializedMsg;
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
void deserializeActorMsg(const folly::IOBuf *buf, MsgT &deserializedMsg) {
    ProtocolT reader;
    reader.setInput(buf);
    deserializedMsg.read(&reader);
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolWriter>
void toIOBuf(const ActorMsg &msg, std::unique_ptr<folly::IOBuf> &buf) {
    serializeActorMsg<MsgT, ProtocolT>(*(std::static_pointer_cast<MsgT>(msg.second)), buf);
}

template <class MsgT, class ProtocolT = apache::thrift::BinaryProtocolReader>
void toActorMsg(const std::unique_ptr<folly::IOBuf> &buf, ActorMsg &msg) {
    std::shared_ptr<MsgT> deserializedMsg = std::make_shared<MsgT>();
    deserializeActorMsg<MsgT, ProtocolT>(buf.get(), *deserializedMsg);
    msg.second = deserializedMsg;
}

}  // namesapce actor

