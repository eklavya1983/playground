#include <actor/ActorMsg.h>

namespace actor {

ActorMsg::ActorMsg() {
}

ActorMsg::ActorMsg(const ActorMsgHeader &h, const Payload &b)
: hdr(h),
buf(b) {
}

ActorMsg::ActorMsg(const ActorMsg &msg)
: hdr(msg.hdr),
    buf(msg.buf) {
}

ActorMsg::ActorMsg(ActorMsg &&msg)
: hdr(std::move(msg.hdr)),
    buf(std::move(msg.buf))
{
}

ActorMsg::~ActorMsg() {
}

/*********************************************************************************
 * Actor message specific
 *********************************************************************************/

SerializerTbl *gMsgTypeInfoTbl = nullptr;

void initCommonActorMsgMappings() {
    ADD_MSGMAPPING(Other,                       1);
    ADD_MSGMAPPING(Init,                        2);
}

void clearActorMappings() {
    gMsgTypeInfoTbl->clear();
    gMsgTypeInfoTbl = nullptr;
}

const char* actorMsgName(ActorMsgTypeId id) {
    return gMsgTypeInfoTbl->at(id).typeName;
}
}  // namespace actor
