import config.ConfigService;
import infra.ConfigTreeUtils;
import infra.InfraException;
import infra.gen.DataSphereInfo;
import infra.gen.NodeInfo;
import infra.gen.ServiceInfo;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.retry.RetryNTimes;
import org.apache.curator.test.TestingServer;
import org.apache.zookeeper.data.Stat;
import org.junit.Before;
import org.junit.Test;

import static org.assertj.core.api.Assertions.assertThatThrownBy;

/**
 * Created by nbayyana on 9/20/16.
 */
public class ConfigServiceTest {
    TestingServer zkServer;
    ConfigService configService;

    @Before
    public void createConfigService() throws Exception {
        zkServer = new TestingServer();
        configService = new ConfigService(zkServer.getConnectString());
        // configService = new ConfigService("192.168.56.102:2181");
        configService.init();
    }

    @Test
    public void basicApis() throws Exception {
        DataSphereInfo sphere = new DataSphereInfo();
        sphere.setId("sphere1");
        configService.addDataSphere(sphere);

        /* Duplicate domain test */
        assertThatThrownBy(()->configService.addDataSphere(sphere))
                .isInstanceOf(InfraException.class);

        /* Test addNode with invalid domain */
        NodeInfo node = new NodeInfo();
        node.setDataSphereId("sphere2");
        node.setId("node1");
        assertThatThrownBy(()->configService.addNode(node))
                .isInstanceOf(InfraException.class);

        /* Test addNode with valid domain */
        node.setDataSphereId("sphere1");
        configService.addNode(node);

        /* adding duplicate node should't work */
        assertThatThrownBy(()->configService.addNode(node))
                .isInstanceOf(InfraException.class);

        /* Test addService with invalid nodeid should fail */
        ServiceInfo service = new ServiceInfo();
        service.setDataSphereId("sphere1");
        service.setNodeId("nodeid2");
        service.setId("service1");
        assertThatThrownBy(()->configService.addService(service))
                .isInstanceOf(InfraException.class);

        /* Test addService with valid info should work */
        service.setNodeId("node1");
        configService.addService(service);

        /* Adding duplicate service should fail */
        assertThatThrownBy(()->configService.addService(service))
                .isInstanceOf(InfraException.class);
    }
}
