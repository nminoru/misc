package jp.nminoru.request_forward_test;

import java.io.IOException;
import java.net.URI;

import org.glassfish.grizzly.http.server.HttpServer;
import org.glassfish.jersey.grizzly2.httpserver.GrizzlyHttpServerFactory;
import org.glassfish.jersey.jsonb.JsonBindingFeature;
import org.glassfish.jersey.media.multipart.MultiPartFeature;
import org.glassfish.jersey.server.ResourceConfig;

public class Main {
    
    public static final String BASE_URI = "http://127.0.0.1:7777/request_forward_test/";
    
    public static HttpServer startServer() {
        final ResourceConfig rc = new ResourceConfig()
            // .packages("jp.nminoru.request_forward_test")
            .packages("jp.nminoru")
            .register(JsonBindingFeature.class)
            .register(MultiPartFeature.class);

        return GrizzlyHttpServerFactory.createHttpServer(URI.create(BASE_URI), rc);
    }
    
    public static void main(String[] args) throws IOException {
        final HttpServer server = startServer();
        
        System.out.println(String.format("Jersey app started with WADL available at "
                + "%sapplication.wadl\nHit enter to stop it...", BASE_URI));
        System.in.read();
        
        server.shutdown();
    }
}
