namespace cpp actor 

typedef i32 ServiceId
typedef i32 ReplicaId
typedef i64 RequestId

struct AsyncHeader {
    1: i32			type;
    2: i64 			from;
    3: i64 			to;
    4: RequestId		requestId;
}

struct ReplicaGroupInfo {
    1: ReplicaId		replicaId;
    2: i32 			version;
    3: list<ServiceId> 		healthyServices;
    4: list<ServiceId> 		unhealthyServices;
}

struct ReplicaRequestHeader {
    1: AsyncHeader		asyncHdr;
    2: ReplicaId		replicaId;
    3: i32 			version;
}

struct ServiceInfo {
    1: string ip;
    2: i32 port;
    3: string type; 
}

service ServiceApi {
    oneway void replicaRequest(1: ReplicaRequestHeader header, 2: binary payload);
    oneway void asyncRequest(1: AsyncHeader header, 2: binary payload);
}

service ConfigApi {
    void registerService(1: ServiceInfo info);
}

service TestService {
  binary reqResp(1: binary req);
  oneway void asyncReq(1: i64 id, 2:binary payload);
  oneway void asyncResp(1: i64 id, 2:binary payload);
}
