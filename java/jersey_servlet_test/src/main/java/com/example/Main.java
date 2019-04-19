package com.example;

import java.io.IOException;
import java.net.URI;
import javax.servlet.Servlet;
import org.glassfish.jersey.grizzly2.httpserver.GrizzlyHttpServerFactory;
import org.glassfish.jersey.server.ResourceConfig;
import org.glassfish.jersey.servlet.ServletContainer;
import org.glassfish.jersey.uri.UriComponent;
import org.glassfish.grizzly.servlet.ServletRegistration;
import org.glassfish.grizzly.servlet.WebappContext;
import org.glassfish.grizzly.http.server.HttpServer;


/**
 * Main class.
 *
 */
public class Main {
    // Base URI the Grizzly HTTP server will listen on
    public static final String BASE_URI = "http://localhost:7777/myapp/";

    /**
     * Starts Grizzly HTTP server exposing JAX-RS resources defined in this application.
     * @return Grizzly HTTP server.
     */
    public static HttpServer createServer() {
        URI uri = URI.create(BASE_URI);
            
        final ResourceConfig rc = new ResourceConfig()
            .packages("com.example");

        ServletContainer servletContainer = new ServletContainer(rc);

        String path = String.format("/%s", UriComponent.decodePath(uri.getPath(), true).get(1).toString());

        WebappContext context = new WebappContext("GrizzlyContext", path);
        ServletRegistration registration;

        registration = context.addServlet("Jersey Servlet", servletContainer);
        registration.addMapping("/*");

        HttpServer httpServer = GrizzlyHttpServerFactory.createHttpServer(uri, false);
        context.deploy(httpServer);
        
        return httpServer;
    }

    /**
     * Main method.
     * @param args
     * @throws IOException
     */
    public static void main(String[] args) throws IOException {
        final HttpServer server = createServer();

        server.start();
        
        System.out.println(String.format("Jersey app started with WADL available at "
                + "%sapplication.wadl\nHit enter to stop it...", BASE_URI));
        System.in.read();
        
        server.shutdownNow();
    }
}

