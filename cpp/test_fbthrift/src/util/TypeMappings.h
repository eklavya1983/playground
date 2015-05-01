#pragma once
#include <actor/ActorMsg.h>


namespace bhoomi {
using namespace actor;
using namespace cpp2;


void initActorMsgMappings() {
    initCommonActorMsgMappings();
    ADD_MSGMAPPING(GetActorRegistry,               3);
    ADD_MSGMAPPING(GetActorRegistryResp,           4);
    ADD_MSGMAPPING(UpdateActorRegistry,            5);
    ADD_MSGMAPPING(UpdateActorInfo,                6);
    ADD_MSGMAPPING(Register,                       7);
    ADD_MSGMAPPING(RegisterResp,                   8);
    ADD_MSGMAPPING(GroupAddVolume,                 9);
    ADD_MSGMAPPING(GroupPutObject,                 10);
    ADD_MSGMAPPING(GroupPutObjectResp,             11);
    ADD_MSGMAPPING(GroupGetObject,                 12);
    ADD_MSGMAPPING(GroupGetObjectResp,             13);
}
}  // namespace bhoomi
