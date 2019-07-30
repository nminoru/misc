package jp.nminoru.jersey_servlet_proxy;

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

import jp.nminoru.jersey_servlet_proxy.ProxyServlet;


public class Main {

    // Base URI the Grizzly HTTP server will listen on
    public static final String BASE_URI   = "http://0.0.0.0:7777/";
    public static       URI    TARGET_URI = URI.create("http://127.0.0.1:32768/");

    public static HttpServer createProxyServer() {
        final ResourceConfig rc = new ResourceConfig()
            .packages("com.example")
            .register(new LoggingFeature(new AccessLog(), Level.INFO, LoggingFeature.Verbosity.PAYLOAD_TEXT, 1000));

        ServletContainer servletContainer = new ServletContainer(rc);

        WebappContext context = new WebappContext("Proxy Servlet", "/");

        ServletRegistration registration;
        registration = context.addServlet("Proxy Servlet", ProxyServlet.class);
        registration.addMapping("/*");

        final URI uri = URI.create(BASE_URI);
        HttpServer httpServer = GrizzlyHttpServerFactory.createHttpServer(uri, false);

        context.deploy(httpServer);

        return httpServer;
    }

    public static void main(String[] args) throws IOException {
        if (args.length < 1) {
            System.out.println("Usage: target-URI");
            return;
        }

        TARGET_URI = URI.create(args[0]);

        final HttpServer server = createProxyServer();

        server.start();

        System.out.println(String.format("Jersey app started with WADL available at "
                + "%sapplication.wadl\nHit enter to stop it...", BASE_URI));
        System.in.read();

        server.shutdownNow();
    }
}

