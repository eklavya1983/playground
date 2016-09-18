/**
 * Created by nbayyana
 */
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import static spark.Spark.get;
import static spark.Spark.post;
import static spark.SparkBase.port;

public class Config {
    private final Logger logger = LoggerFactory.getLogger(Config.class);

    public static void main(String args[]) {
        port(8080);
        get("/status", (request, response) -> {return "ok";});


        post("/v1/datasphere", (request, response) -> {
            return "ok";
        });
        post("/v1/datasphere/:dsid/node", (request, response) -> {
            return "ok";
        });

        /* Storage ring operations */
        post("/v1/datasphere/:dsid/storagering", (request, response) -> {
            return "ok";
        });
        post("/v1/datasphere/:dsid/storagering/node", (request, response) -> {
            return "ok";
        });
        post("/v1/datasphere/:dsid/storagering/pod", (request, response) -> {
            return "ok";
        });

        /* Dataring operations */
        post("/v1/datasphere/:dsid/dataring", (request, response) -> {
            return "ok";
        });
        post("/v1/datasphere/:dsid/dataring/node", (request, response) -> {
            return "ok";
        });
        post("/v1/datasphere/:dsid/dataring/volume", (request, response) -> {
            return "ok";
        });
    }
}
