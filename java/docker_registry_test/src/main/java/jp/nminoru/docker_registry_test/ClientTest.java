package jp.nminoru.docker_registry_test;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import jakarta.ws.rs.client.Client;
import jakarta.ws.rs.client.ClientBuilder;
import jakarta.ws.rs.client.Invocation;
import jakarta.ws.rs.client.WebTarget;
import jakarta.ws.rs.core.HttpHeaders;
import jakarta.ws.rs.core.Response;
import org.glassfish.jersey.client.ClientProperties;
import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import org.glassfish.jersey.jsonb.JsonBindingFeature;

import jp.nminoru.docker_registry_test.model.JwtResponse;
import jp.nminoru.docker_registry_test.model.Repositories;
import jp.nminoru.docker_registry_test.model.Tags;


public class ClientTest {

    public final static Pattern pattern1 = Pattern.compile("Bearer realm=\"(.+)\",service=\"(.+)\",scope=\"(.+)\",error=\".+\"");
    public final static Pattern pattern2 = Pattern.compile("Bearer realm=\"(.+)\",service=\"(.+)\",scope=\"(.+)\"");
    public final static Pattern pattern3 = Pattern.compile("Bearer realm=\"(.+)\",service=\"(.+)\"");    

    public final static String username   = "<user>";
    public final static String password   = "<password>";

    public final static String serverUrl = "http://registry.example.com/";

    String authorization;

    class UnauthorizedException extends Exception {
        Response response;
        
        UnauthorizedException(Response response) {
            this.response = response;
        }
    }
    
    public interface Code<T> {
        public T call(String authorization) throws UnauthorizedException;
    }

    public <T> T executeCode(Code<T> code) {
        try {
            return code.call(authorization);
        } catch (UnauthorizedException e1) {
            // WWW-Authenticate ヘッダーを解析してトークンサーバーを探す
            Response errorResponse = e1.response;

            String authenticate = errorResponse.getHeaderString(HttpHeaders.WWW_AUTHENTICATE);

            if (authenticate == null)
                throw new RuntimeException("");

            Matcher m;

            String realm   = null;
            String service = null;
            String scope   = null;

            m = pattern1.matcher(authenticate);
            if (m.matches()) {
                realm   = m.group(1);                
                service = m.group(2);
                scope   = m.group(3);
            } else {
                m = pattern2.matcher(authenticate);
                
                if (m.matches()) {
                    realm   = m.group(1);                
                    service = m.group(2);
                    scope   = m.group(3);                    
                } else {

                    m = pattern3.matcher(authenticate);
                
                    if (m.matches()) {
                        realm   = m.group(1);                
                        service = m.group(2);                        
                    } else {
                        throw new RuntimeException("");
                    }
                }
            }

            // System.out.println("realm: " + realm);
            // System.out.println("service: " + service);
            // System.out.println("scope: " + scope);
            
            String url;
            String path;
            try {
                URI uri1 = URI.create(realm);                
                URI uri2 = new URI(uri1.getScheme(), null, uri1.getHost(), uri1.getPort(), null, null, null);
                
                url  = uri2.toASCIIString();
                path = uri1.getPath();
            } catch (URISyntaxException ue) {
                ue.printStackTrace();
                throw new RuntimeException("");
            }

            // トークンサーバーへ要求
            Client client = ClientBuilder.newBuilder()
                .register(JsonBindingFeature.class)
                .register(HttpAuthenticationFeature.basic(username, password))
                .build();

            WebTarget target = client
                .target(url)
                .path(path)
                .queryParam("service",   service)
                .queryParam("client_id", "client");

            if (scope != null)
                target = target.queryParam("scope", scope);

            Response response = target.request().get();

            if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
                throw new RuntimeException("");

            JwtResponse jwtRes = response.readEntity(JwtResponse.class);

            authorization = "Bearer " + jwtRes.token;

            try {
                return code.call(authorization);
            } catch (UnauthorizedException e2) {
                System.out.println("WWW-Authenticate: " + authenticate);
                System.out.println("realm: " + realm);
                System.out.println("service: " + service);
                System.out.println("scope: " + scope);                
                throw new RuntimeException("");
            }
        }
    }

    void run() {

        Client client = ClientBuilder.newBuilder()
            .register(JsonBindingFeature.class)
            .build();

        Repositories repositories = executeCode((authorization) ->
            {
                Invocation.Builder builder = client
                    .target(serverUrl)
                    .path("/v2/_catalog")
                    .request();

                if (authorization != null)
                    builder = builder.header(HttpHeaders.AUTHORIZATION, authorization);

                Response response = builder.get();

                if (response.getStatus() == Response.Status.UNAUTHORIZED.getStatusCode())
                    throw new UnauthorizedException(response);

                return response.readEntity(Repositories.class);
            });

        for (String repository : repositories.repositories) {

            Tags tags = executeCode((authorization) ->
                {
                    Invocation.Builder builder = client
                        .target(serverUrl)
                        .path("/v2/" + repository + "/tags/list")
                        .request();

                if (authorization != null)
                    builder = builder.header(HttpHeaders.AUTHORIZATION, authorization);

                Response response = builder.get();

                if (response.getStatus() == Response.Status.UNAUTHORIZED.getStatusCode())
                    throw new UnauthorizedException(response);

                return response.readEntity(Tags.class);
            });

            if (tags.tags != null) {
                for (String tag : tags.tags) {
                    System.out.println(repository + "\t" + tag);
                }
            }
        }
    }

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        clientTest.run();
    }
}
