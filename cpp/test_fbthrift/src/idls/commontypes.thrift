namespace cpp infra 
namespace java infra.gen

struct DataSphereInfo {
	1: string id;
}

struct NodeInfo {
	1: string id;
	2: string dataSphereId;
	3: list<string> serviceIds;
}

struct ServiceInfo {
	1: string id;
	2: string dataSphereId;
	3: string nodeId;
}
