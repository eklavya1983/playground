package config;

import infra.ConfigTreeUtils;
import infra.InfraException;
import infra.gen.DataSphereInfo;
import infra.gen.NodeInfo;
import infra.gen.ServiceInfo;
import infra.gen.Status;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.retry.RetryNTimes;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.data.Stat;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ConfigService {
    public final Logger logger = LoggerFactory.getLogger(ConfigService.class);
    private CuratorFramework client;

    public ConfigService(String serverString) {
       /* Setup Curator client */
        client = CuratorFrameworkFactory.builder()
                .connectString(serverString)
                .retryPolicy(new RetryNTimes(1, 1000))
                .namespace("dataom")
                .build();
    }

    public void init()
    {
        logger.info("Initializing...");
        startCurator();
    }

    public void startCurator()
    {
        logger.info("Initializing curator");
        client.start();
        try {
            logger.info("Waiting to connect to zookeeper...");
            client.blockUntilConnected();
            logger.info("Connected to zookeeper");
        } catch (InterruptedException e) {
            e.printStackTrace();
            logger.error("exiting....");
            System.exit(1);
        }
    }

    public void addDataSphere(DataSphereInfo info) throws InfraException {
        try {
            String path = ConfigTreeUtils.getDataSpherePath(info.getId());
            client.create().creatingParentsIfNeeded().forPath(path);
            logger.debug("created datasphere path: " + path);
        } catch (KeeperException.NodeExistsException e) {
            logger.warn(e.toString());
            throw new InfraException(Status.STATUS_DUPLICATE_DATASPHERE);
        } catch (Exception e) {
            logger.warn(e.toString());
            throw new InfraException(Status.STATUS_INVALID);
        }
    }

    public void addNode(NodeInfo info) throws Exception {
        /* Ensure datasphere exists */
        throwIfDataSphereIsInvalid(info.getDataSphereId());

        try {
            /* Create node */
            String path = ConfigTreeUtils.getNodePath(info.getDataSphereId(), info.getId());
            client.create().creatingParentsIfNeeded().forPath(path);
            logger.debug("created node path: " + path);
        } catch (KeeperException.NodeExistsException e) {
            logger.warn(e.toString());
            throw new InfraException(Status.STATUS_DUPLICATE_NODE);
        } catch (Exception e) {
            logger.warn(e.toString());
            throw new InfraException(Status.STATUS_INVALID);
        }
    }

    public void addService(ServiceInfo info) throws Exception {
        /* Ensure datasphere and node exists */
        throwIfNodeIsInvalid(info.getDataSphereId(), info.getNodeId());

        try {
            /* Create Service */
            String path = ConfigTreeUtils.getServicePath(info.getDataSphereId(), info.getId());
            client.create().creatingParentsIfNeeded().forPath(path);
            logger.debug("created service path: " + path);
        } catch (KeeperException.NodeExistsException e) {
            logger.warn(e.toString());
            throw new InfraException(Status.STATUS_DUPLICATE_NODE);
        } catch (Exception e) {
            logger.warn(e.toString());
            throw new InfraException(Status.STATUS_INVALID);
        }
    }

    protected void throwIfDataSphereIsInvalid(String dataSphereId) throws Exception {
            String path = ConfigTreeUtils.getDataSpherePath(dataSphereId);
            Stat stat = client.checkExists().forPath(path);
            if (stat == null) {
                throw new InfraException(Status.STATUS_INVALID_DATASPHERE);
            }
    }

    protected void throwIfNodeIsInvalid(String dataSphereId, String nodeId) throws Exception {
        Stat stat = client.checkExists().forPath(ConfigTreeUtils.getNodePath(dataSphereId, nodeId));
        if (stat == null) {
            throw new InfraException(Status.STATUS_INVALID_NODE);
        }
    }

    protected void throwIfServiceIsInvalid(String dataSphereId, String nodeId, String serviceId) throws Exception {
        throwIfNodeIsInvalid(dataSphereId, nodeId);
        Stat stat = client.checkExists().forPath(ConfigTreeUtils.getServicePath(dataSphereId, serviceId));
        if (stat == null) {
            throw new InfraException(Status.STATUS_INVALID_SERVICE);
        }
    }

    public static void main(String args[])
    {
        ConfigService configService = new ConfigService("localhost:2181");
        configService.init();
        new Webapp(configService).start();
    }
}
