package jp.nminoru.docker_registry_test;

import jakarta.ws.rs.client.Client;
import jakarta.ws.rs.client.ClientBuilder;
import jakarta.ws.rs.client.WebTarget;
import jakarta.ws.rs.core.HttpHeaders;;
import jakarta.ws.rs.core.Response;
import org.glassfish.jersey.client.ClientProperties;
import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import org.glassfish.jersey.jsonb.JsonBindingFeature;

import jp.nminoru.docker_registry_test.model.JwtResponse;


public class ClientTest {

    public final static String serverUrl1 = "http://gitlab.example.com/";
    public final static String username   = "<username>";
    public final static String password   = "<password>";

    public final static String serverUrl2 = "http://registry.example.com/";

    public static void main(String[] args) {

        Client client1 = ClientBuilder.newBuilder()
            .register(JsonBindingFeature.class)
            .register(HttpAuthenticationFeature.basic(username, password))
            .build();

        Response response1 = client1
            .target(serverUrl1)
            .path("/jwt/auth")
            .queryParam("service",   "container_registry")
            .queryParam("client_id", "client")
            .queryParam("scope",     "registry:catalog:*")
            .request()
            .get();

        if (!response1.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response1.readEntity(String.class));
            return;
        }

        JwtResponse jwtRes = response1.readEntity(JwtResponse.class);

        Client client2 = ClientBuilder.newBuilder()
            .register(JsonBindingFeature.class)
            .build();        

        Response response2 = client2
            .target(serverUrl2)
            .path("/v2/_catalog")
            .request()
            .header(HttpHeaders.AUTHORIZATION, "Bearer " + jwtRes.token)            
            .get();

        if (!response2.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response2.readEntity(String.class));
            return;
        }

        System.err.println(response2.readEntity(String.class));
    }
}
