#include <actor/ActorMsg.h>
#include <actor/ActorSystem.h>      // Only included for ActorSystem::invalidActorId()

namespace actor {

const RequestId ActorMsg::UNTRACKED_ID;

ActorMsg::ActorMsg() {
    hdr.direction = MSGDIRECTION_NORMAL;
    hdr.typeId = ActorMsg::INVALID_MSGTYPEID;
    hdr.from = ActorSystem::invalidActorId();
    hdr.to = ActorSystem::invalidActorId();
    hdr.requestId = ActorMsg::UNTRACKED_ID;
    hdr.status = static_cast<int>(Error::ERR_OK);
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

void initActorSystemMappings() {
    initCommonActorMsgMappings();
    ADD_MSGMAPPING(GetActorRegistry,               3);
    ADD_MSGMAPPING(GetActorRegistryResp,           4);
    ADD_MSGMAPPING(UpdateActorRegistry,            5);
    ADD_MSGMAPPING(UpdateActorInfo,                6);
}

void clearActorMappings() {
    gMsgTypeInfoTbl->clear();
    gMsgTypeInfoTbl = nullptr;
}

const char* actorMsgName(ActorMsgTypeId id) {
    return gMsgTypeInfoTbl->at(id).typeName;
}
}  // namespace actor
