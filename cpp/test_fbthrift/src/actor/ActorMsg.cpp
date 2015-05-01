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
template<class T>
ActorMsgTypeId ActorMsgTypeInfo<T>::typeId = ActorMsg::INVALID_MSGTYPEID;

SerializerTbl *gMsgMapTbl = nullptr;

void initActorMsgMappings() {
#define ADD_MAPPING(type) addActorMsgMapping<type>(ActorMsgTypeIds::type##Msg)
    ADD_MAPPING(Other);
    ADD_MAPPING(Init);
    ADD_MAPPING(GetActorRegistry);
    ADD_MAPPING(GetActorRegistryResp);
    ADD_MAPPING(UpdateActorRegistry);
    ADD_MAPPING(UpdateActorInfo);
    ADD_MAPPING(Register);
    ADD_MAPPING(RegisterResp);
    ADD_MAPPING(GroupAddVolume);
    ADD_MAPPING(GroupPutObject);
    ADD_MAPPING(GroupPutObjectResp);
    ADD_MAPPING(GroupGetObject);
    ADD_MAPPING(GroupGetObjectResp);

}

void clearActorMappings() {
    gMsgMapTbl->clear();
    gMsgMapTbl = nullptr;
}

const char* actorMsgName(ActorMsgTypeId id) {
    return _ActorMsgTypeIds_VALUES_TO_NAMES.at(static_cast<ActorMsgTypeIds>(id));
}
}  // namespace actor
