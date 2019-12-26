package jp.nminoru.jersey_jaxb_test;

import java.util.List;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.WebTarget;
import org.glassfish.grizzly.http.server.HttpServer;


public class MyResourceTest {

    protected HttpServer server;
    protected WebTarget  target;

    protected void setUp(Class feature) throws Exception {
        server = Main.startServer();

        Client client = ClientBuilder.newBuilder()
            .register(feature)
            .build();

        target = client.target(Main.BASE_URI);
    }

    protected void tearDown() throws Exception {
        server.shutdownNow();
    }
}
