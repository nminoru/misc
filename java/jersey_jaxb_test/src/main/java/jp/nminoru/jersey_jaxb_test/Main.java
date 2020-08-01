package jp.nminoru.jersey_jaxb_test;

import java.io.IOException;
import java.net.URI;
import org.glassfish.grizzly.http.server.HttpServer;
import org.glassfish.jersey.grizzly2.httpserver.GrizzlyHttpServerFactory;
import org.glassfish.jersey.server.ResourceConfig;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.moxy.json.MoxyJsonFeature;
import org.glassfish.jersey.jsonb.JsonBindingFeature;


public class Main {

    public static final String BASE_URI = "http://localhost:8888/";

    public static HttpServer startServer(Class feature) {
        final ResourceConfig rc = new ResourceConfig()
            .packages("jp.nminoru.jersey_jaxb_test")
            .register(feature);

            // 下のうち1つを選択する
            // .register(MoxyJsonFeature.class)
            // .register(JacksonFeature.class)
            // .register(JsonBindingFeature.class)

        return GrizzlyHttpServerFactory.createHttpServer(URI.create(BASE_URI), rc);
    }

    public static void main(String[] args) throws IOException {
        final HttpServer server = startServer(MoxyJsonFeature.class);
        System.out.println(String.format("Jersey app started with WADL available at "
                + "%sapplication.wadl\nHit enter to stop it...", BASE_URI));
        System.in.read();
        server.shutdown();
    }
}


