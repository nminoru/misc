package jp.nminoru.docker_registry_test;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.List;
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

    public final static String  URL      = "DOCKER_REGISTRY_URL";    
    public final static String  USER     = "DOCKER_REGISTRY_USER";
    public final static String  PASSWORD = "DOCKER_REGISTRY_PASSWORD";

    public final static Pattern pattern1 = Pattern.compile("Bearer realm=\"(.+)\",service=\"(.+)\",scope=\"(.+)\",error=\".+\"");
    public final static Pattern pattern2 = Pattern.compile("Bearer realm=\"(.+)\",service=\"(.+)\",scope=\"(.+)\"");
    public final static Pattern pattern3 = Pattern.compile("Bearer realm=\"(.+)\",service=\"(.+)\"");

    public final static Pattern commentLinePattern = Pattern.compile("^\\s*#.*$");
    public final static Pattern tagLinePattern     = Pattern.compile("(.+)\t(.+)\t(.+)");

    public static String serverUrl = "http://registry.example.com/";    
    public static String username   = "<user>";
    public static String password   = "<password>";

    String authorization;

    class UnauthorizedException extends Exception {
        Response response;
        
        UnauthorizedException(Response response) {
            this.response = response;
        }
    }

    static class TokenServerInfo {
        TokenServerInfo(String realm, String service, String scope) {
            this.realm   = realm;
            this.service = service;
            this.scope   = scope;
        }
        
        TokenServerInfo(String realm, String service) {
            this(realm, service, null);
        }        
        
        public String realm;
        public String service;
        public String scope;
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

            String authenticateString = errorResponse.getHeaderString(HttpHeaders.WWW_AUTHENTICATE);

            if (authenticateString == null)
                throw new RuntimeException("WWW-Authenticate header is not found");
            
            TokenServerInfo info = parseWwwAuthenticate(authenticateString);

            String url;
            String path;
            try {
                URI uri1 = URI.create(info.realm);                
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
                .queryParam("service",   info.service)
                .queryParam("client_id", "client");

            if (info.scope != null)
                target = target.queryParam("scope", info.scope);

            Response response = target.request().get();

            if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
                throw new RuntimeException("");

            JwtResponse jwtRes = response.readEntity(JwtResponse.class);

            authorization = "Bearer " + jwtRes.token;

            try {
                return code.call(authorization);
            } catch (UnauthorizedException e2) {
                System.out.println("WWW-Authenticate: " + authenticateString);
                throw new RuntimeException("");
            }
        }
    }

    TokenServerInfo parseWwwAuthenticate(String authenticateString) {
        Matcher m;

        String realm   = null;
        String service = null;
        String scope   = null;

        m = pattern1.matcher(authenticateString);
        if (m.matches())
            return new TokenServerInfo(m.group(1), m.group(2), m.group(3));

        m = pattern2.matcher(authenticateString);
        if (m.matches())
            return new TokenServerInfo(m.group(1), m.group(2), m.group(3));

        m = pattern3.matcher(authenticateString);
        if (m.matches())
            return new TokenServerInfo(m.group(1), m.group(2));

        throw new RuntimeException("cannnot parse the content of the WWW-Authenticate header");
    }

    Client client = ClientBuilder.newBuilder()
        .register(JsonBindingFeature.class)
        .build();

    void lookupTags() {

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

                    String digest = executeCode((authorization) ->
                        {
                            Invocation.Builder builder = client
                                .target(serverUrl)
                                .path("/v2/" + repository + "/manifests/" + tag)
                                .request();

                            if (authorization != null)
                                builder = builder.header(HttpHeaders.AUTHORIZATION, authorization);

                            Response response = builder.head();
                            
                            if (response.getStatus() == Response.Status.UNAUTHORIZED.getStatusCode())
                                throw new UnauthorizedException(response);

                            String ret = response.getHeaderString("Docker-Content-Digest");

                            return ret;
                        });                    
                    
                    System.out.println(repository + "\t" + tag + "\t" + digest);
                }
            }
        }
    }

    void deleteTags() throws IOException {
        List<String> lines = new ArrayList<>();
        
        // 標準入力を1行ずつ読み出す。
        BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));        
        String line;
        while ((line = stdin.readLine()) != null) {
            
            // コメント行ならスキップ            
            Matcher m = commentLinePattern.matcher(line);
            if (m.matches())
                continue;

            lines.add(line);
        }

        for (String tagLine : lines) {
            Matcher m = tagLinePattern.matcher(tagLine);

            if (m.matches()) {
                String repository = m.group(1);
                String tag        = m.group(3);
                
                executeCode((authorization) ->
                        {
                            Invocation.Builder builder = client
                                .target(serverUrl)
                                .path("/v2/" + repository + "/manifests/" + tag)
                                .request();

                            if (authorization != null)
                                builder = builder.header(HttpHeaders.AUTHORIZATION, authorization);

                            Response response = builder.delete();
                            
                            if (response.getStatus() == Response.Status.UNAUTHORIZED.getStatusCode())
                                throw new UnauthorizedException(response);

                            if (response.getStatus() != Response.Status.ACCEPTED.getStatusCode())
                                throw new RuntimeException("");

                            return (Void)null;
                        });                                    
                
            }
        }
    }

    public static void main(String[] args) throws IOException {
        // 環境変数の読み込み
        serverUrl = System.getenv(URL);
        username  = System.getenv(USER);
        password  = System.getenv(PASSWORD);
        
        ClientTest clientTest = new ClientTest();

        String command = (args.length >= 1) ? args[0] : "";

        switch (command) {

            case "--list-tags":
            default:        
                clientTest.lookupTags();
                break;

            case "--delete-tags":
                clientTest.deleteTags();
                break;
        }
    }
}
