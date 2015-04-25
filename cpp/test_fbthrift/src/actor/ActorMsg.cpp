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

template<class T>
ActorMsgTypeId ActorMsgTypeInfo<T>::typeId;

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
    ADD_MAPPING(AddVolume);
    ADD_MAPPING(PutObject);
    ADD_MAPPING(PutObjectResp);
    ADD_MAPPING(GetObject);
    ADD_MAPPING(GetObjectResp);

}

void clearActorMappings() {
    gMsgMapTbl->clear();
    gMsgMapTbl = nullptr;
}
}  // namespace actor
