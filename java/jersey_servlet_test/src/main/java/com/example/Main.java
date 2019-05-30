package com.example;

import java.io.IOException;
import java.net.URI;
import java.util.logging.Level;
import javax.servlet.Servlet;
import org.glassfish.jersey.grizzly2.httpserver.GrizzlyHttpServerFactory;
import org.glassfish.jersey.logging.LoggingFeature;
import org.glassfish.jersey.server.ResourceConfig;
import org.glassfish.jersey.servlet.ServletContainer;
import org.glassfish.grizzly.servlet.ServletRegistration;
import org.glassfish.grizzly.servlet.WebappContext;
import org.glassfish.grizzly.http.server.HttpServer;


/**
 * Main class.
 *
 */
public class Main {
    // Base URI the Grizzly HTTP server will listen on
    public static final String BASE_URI = "http://0.0.0.0:7777/";

    /**
     * Starts Grizzly HTTP server exposing JAX-RS resources defined in this application.
     * @return Grizzly HTTP server.
     */
    public static HttpServer createServer() {
        URI uri = URI.create(BASE_URI);
            
        final ResourceConfig rc1 = new ResourceConfig()
            .packages("com.example")
            .register(new LoggingFeature(new AccessLog(), Level.INFO, LoggingFeature.Verbosity.PAYLOAD_TEXT, 1000));
        
        ServletContainer servletContainer1 = new ServletContainer(rc1);

        WebappContext context1 = new WebappContext("GrizzlyContext", "/path1");
        ServletRegistration registration1;

        registration1 = context1.addServlet("Jersey Servlet1", servletContainer1);
        registration1.addMapping("/*");

        final ResourceConfig rc2 = new ResourceConfig()
            .packages("com.example")
            .register(new LoggingFeature(new AccessLog(), Level.INFO, LoggingFeature.Verbosity.PAYLOAD_TEXT, 1000));        

        ServletContainer servletContainer2 = new ServletContainer(rc2);        

        WebappContext context2 = new WebappContext("GrizzlyContext", "/path2");
        ServletRegistration registration2;

        registration2 = context2.addServlet("Jersey Servlet2", servletContainer2);
        registration2.addMapping("/*");

        HttpServer httpServer = GrizzlyHttpServerFactory.createHttpServer(uri, false);

        context1.deploy(httpServer);
        context2.deploy(httpServer);
        
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

