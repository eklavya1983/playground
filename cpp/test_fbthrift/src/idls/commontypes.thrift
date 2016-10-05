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

/* Common keys */
const string KEY_VERSION                        = "version"
const string KEY_TYPE                           = "type"
const string KEY_ID				= "id"

/* Holds binary data with some properties.  Typical properties include type, version, etc. */
struct KVBinaryData {
	1: map<string, string>  props;
	2: binary               data;
}

struct Properties {
	1: map<string, string>		props;
}
