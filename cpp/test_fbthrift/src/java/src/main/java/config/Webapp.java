package config;

import com.google.gson.Gson;
import com.google.gson.JsonObject;
import infra.gen.DataSphereInfo;

import static spark.Spark.get;
import static spark.Spark.post;
import static spark.SparkBase.port;

/**
 * Created by nbayyana on 9/19/16.
 */
public class Webapp {
    private final ConfigService configService;
    private Gson gson = new Gson();

    public Webapp(ConfigService configService) {
        this.configService = configService;
    }

    public void start() {
        port(8080);
        get("/status", (request, response) -> {return "ok";});


        post("/v1/datasphere", (request, response) -> {
            JsonObject json = gson.fromJson(request.body(), JsonObject.class);
            DataSphereInfo info = new DataSphereInfo();
            info.setId(json.get("id").getAsString());
            configService.addDataSphere(info);
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
