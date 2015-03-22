#include <actor/ActorMsg.h>

namespace actor {

template<class T>
ActorMsgTypeId ActorMsgTypeInfo<T>::typeId;

SerializerTbl *gMsgMapTbl;

void initSerializers() {
#define ADD_MAPPING(type) addActorMsgMapping<type>(ActorMsgTypeIds::type##Msg)
    ADD_MAPPING(Other);
    ADD_MAPPING(Init);
    ADD_MAPPING(GetActorRegistry);
    ADD_MAPPING(GetActorRegistryResp);
    ADD_MAPPING(UpdateActorRegistry);
    ADD_MAPPING(UpdateActorInfo);
    ADD_MAPPING(RegisterActorSystem);
    ADD_MAPPING(RegisterActorSystemResp);
    ADD_MAPPING(AddVolume);
    ADD_MAPPING(PutObject);
    ADD_MAPPING(PutObjectResp);
    ADD_MAPPING(GetObject);
    ADD_MAPPING(GetObjectResp);

}

}  // namespace actor
