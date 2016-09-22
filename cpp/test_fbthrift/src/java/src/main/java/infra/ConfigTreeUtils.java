package infra;

import infra.gen.configtreeConstants;

/**
 * Created by nbayyana on 9/20/16.
 */
public final class ConfigTreeUtils {
    static public String makePath(String... args) {
        StringBuilder builder = new StringBuilder();
        for (String arg : args) {
            builder.append("/");
            builder.append(arg);
        }
        return builder.toString();
    }

    static public String getDataSpherePath(String dataSphereId) {
       return makePath(
               configtreeConstants.DATASPHERES_ROOT,
               dataSphereId);
    }
    static public String getNodePath(String dataSphereId, String nodeId) {
        return makePath(
                configtreeConstants.DATASPHERES_ROOT,
                dataSphereId,
                configtreeConstants.NODES_ROOT,
                nodeId);
    }
    static public String getServicePath(String dataSphereId, String serviceId) {
        return makePath(
                configtreeConstants.DATASPHERES_ROOT,
                dataSphereId,
                configtreeConstants.SERVICES_ROOT,
                serviceId);
    }
}
