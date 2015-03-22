namespace cpp actor

typedef i32 ActorSystemId
typedef i32 LocalActorId
typedef i32 ActorMsgType
typedef i64 RequestId
typedef binary (cpp.type = "folly::IOBuf") ActorMsgBuffer

struct ActorId {
    1: ActorSystemId systemId;
    2: LocalActorId localId;
}

struct ActorMsgHeader {
    1: ActorMsgType		type;
    2: ActorId			from;
    3: ActorId			to;
    4: RequestId		requestId;
}

typedef i32 ReplicaId
struct ReplicaGroupInfo {
    1: LocalActorId			replicaId;
    2: i32 				version;
    3: list<ActorSystemId> 		replicaServices;
    4: list<ActorSystemId> 		healthyServices;
    5: list<ActorSystemId> 		unhealthyServices;
}

struct ReplicaRequestHeader {
    1: ActorMsgHeader		actorHdr;
    2: ReplicaId		replicaId;
    3: i32 			version;
}

struct ActorInfo {
    1: string			type;
    2: ActorId			id; 
    3: string			ip; 
    4: i32			port;
    5: i32			incarnation;
}

service ServiceApi {
    oneway void replicaRequest(1: ReplicaRequestHeader header, 2: binary payload);
    oneway void actorMessage(1: ActorMsgHeader header, 2: ActorMsgBuffer payload);
}

/*-----------------------------------------------------------
 * Common actor messages 
 *-----------------------------------------------------------*/
enum ActorMsgTypes {
    COMMON_MSG_BEGIN		= 0,
    OtherMsg			= 1,
    InitMsg			= 2,
    GetActorRegistryMsg		= 3,
    GetActorRegistryRespMsg	= 4,
    UpdateActorRegistryMsg	= 5,
    COMMON_MSG_END		= 999,
}

struct GetActorRegistry {
}

typedef list<ActorInfo> GetActorRegistryResp

struct UpdateActorRegistry {
}

/*-----------------------------------------------------------
 * Config messages 
 *-----------------------------------------------------------*/
enum ConfigMsgTypes {
    CONFIG_MSG_BEGIN 		= 1000,
    RegisterActorSystemMsg 	= 1001,
    RegisterActorSystemRespMsg 	= 1002,
    CONFIG_MSG_END 		= 1999,
}

struct RegisterActorSystem {
    1: ActorInfo		systemInfo;
}
struct RegisterActorSystemResp {
}

/*-----------------------------------------------------------
 * Data acess messages 
 *-----------------------------------------------------------*/
enum DataAcessMsgTypes {
    DATAACESS_MSG_BEGIN 	= 2000,
    AddVolumeMsg 		= 2001,
    PutObjectMsg		= 2002,
    PutObjectRespMsg		= 2003,
    GetObjectMsg		= 2004,
    GetObjectRespMsg		= 2005,
    DATAACESS_MSG_END 		= 2999,
}

struct AddVolume {
    1: ReplicaGroupInfo 	replicaInfo;
    2: i64			volumeId;
    3: string 			volumeName;
}
struct PutObject {
}
struct PutObjectResp {
}
struct GetObject {
}
struct GetObjectResp {
}

service TestService {
  binary reqResp(1: binary req);
  oneway void asyncReq(1: i64 id, 2:binary payload);
  oneway void asyncResp(1: i64 id, 2:binary payload);
}
