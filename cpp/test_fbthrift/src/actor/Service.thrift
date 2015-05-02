namespace cpp actor

typedef i32 ActorSystemId
typedef i32 LocalActorId
typedef i32 ActorMsgTypeId
typedef i64 RequestId
typedef binary (cpp.type = "folly::IOBuf") ActorMsgBuffer

struct ActorId {
    1: ActorSystemId systemId;
    2: LocalActorId localId;
}

struct ActorMsgHeader {
    1: ActorMsgTypeId		typeId;
    2: ActorId			from;
    3: ActorId			to;
    4: RequestId		requestId;
    5: int			status; 
}

typedef i32 ReplicaId
struct ReplicaGroupInfo {
    1: LocalActorId			replicaId;
    2: i32 				version;
    3: list<ActorSystemId> 		replicaServices;
    4: list<ActorSystemId> 		healthyServices;
    5: list<ActorSystemId> 		unhealthyServices;
}
typedef ReplicaGroupInfo (cpp.type = "std::unique_ptr<ReplicaGroupInfo>") ReplicaGroupInfoptr

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
    oneway void trackedActorMessageResponse(1: ActorMsgHeader header, 2: ActorMsgBuffer payload);
}
/*-----------------------------------------------------------
 * Errors
 *-----------------------------------------------------------*/
enum Error {
    ERR_OK			= 0,
    ERR_INVALID			= 1
}
/*-----------------------------------------------------------
 * Common actor messages 
 *-----------------------------------------------------------*/
struct Init {}
struct Other {}

struct GetActorRegistry {
}

struct GetActorRegistryResp {
    1: list<ActorInfo> infoList;
}

struct UpdateActorRegistry {
}

struct UpdateActorInfo {
    1: ActorInfo info;
}

/*-----------------------------------------------------------
 * Config messages 
 *-----------------------------------------------------------*/
struct Register {
    1: ActorInfo		systemInfo;
}
struct RegisterResp {
    1: i32			error;
    2: ActorId			id;
}

/*-----------------------------------------------------------
 * Data acess messages 
 *-----------------------------------------------------------*/
struct GroupAddVolume {
    1: ReplicaGroupInfo		replicaInfo;
    2: i64			volumeId;
    3: string 			volumeName;
}
struct GroupPutObject {
}
struct GroupPutObjectResp {
}
struct GroupGetObject {
}
struct GroupGetObjectResp {
}

service TestService {
  binary reqResp(1: binary req);
  oneway void asyncReq(1: i64 id, 2:binary payload);
  oneway void asyncResp(1: i64 id, 2:binary payload);
}
