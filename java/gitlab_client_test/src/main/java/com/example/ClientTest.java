package com.example;

import java.util.ArrayList;
import java.util.List;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.MediaType;

import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import static org.junit.Assert.*;

public class ClientTest {

    public static void main(String[] args) {
        ClientTest c = new ClientTest();

        c.testProjects();
    }

    public WebTarget generateTarget() {
        Client client = ClientBuilder.newBuilder()
            .build();

        return client.target("http://127.0.0.1:80/api/v4/");
    }

    public void testProjects() {
        WebTarget target = generateTarget();

        String response = target
            .path("/projects")
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .get(String.class);
        
        // assertTrue(response.success);

        System.out.println(response);
    }
}

