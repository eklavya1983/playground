namespace cpp infra 
namespace java infra.gen

struct DataSphereInfo {
	1: string 			id;
}

struct NodeInfo {
	1: string 			id;
	2: string 			dataSphereId;
	3: list<string> 		serviceIds;
}

struct ServiceInfo {
	1: string 			id;
	2: string 			dataSphereId;
	3: string 			nodeId;
	4: string 			ip;
	5: i32				port;
}

const i32 INVALID_VERSION = -1;
struct VersionedData {
	1: i64				version;
	2: string 			data;
}

struct Properties {
	1: map<string, string>		props;
}
