package jp.nminoru.request_forward_test;

import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.util.List;
import java.util.Map;
import javax.ws.rs.Consumes;
import javax.ws.rs.DELETE;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.PUT;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.container.ContainerRequestContext;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.MultivaluedMap;
import javax.ws.rs.core.Request;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;
import org.glassfish.jersey.jsonb.JsonBindingFeature;
import org.glassfish.jersey.media.multipart.MultiPartFeature;

import jp.nminoru.request_forward_test.Main;
import jp.nminoru.request_forward_test.Model;

@Path("/resource")
public class Resource {
    @Path("/ping")
    @GET
    @Produces(MediaType.TEXT_PLAIN)    
    public String getPing() {
        return "ping";
    }    

    @Path("/test")
    @GET
    @Produces(MediaType.TEXT_PLAIN)
    public String getTest(String message) {
        return message;
    }    

    @Path("/test")
    @POST
    @Consumes(MediaType.TEXT_PLAIN)
    @Produces(MediaType.TEXT_PLAIN)
    public String postTest(String message) {
        return message;
    }

    @Path("/nano")
    @POST    
    public Response nano() {
        return Response.status(200).entity("hoge").type(MediaType.TEXT_PLAIN).build();        
    }
    
    @Path("/forward")
    @POST
    public Response forwardRequestViaPOST(@Context ContainerRequestContext requestContext) throws IOException {
        return forwardRequest("/resource/test", requestContext);
    }

    @Path("/forward")
    @GET
    public Response forwardRequestViaGET(@Context ContainerRequestContext requestContext) throws IOException {
        return forwardRequest("/resource/test", requestContext);
    }    

    Response forwardRequest(String path, ContainerRequestContext requestContext) throws IOException {
        Client client = ClientBuilder.newBuilder()
            .register(MultiPartFeature.class)
            .register(JsonBindingFeature.class)            
            .build();

        String method = requestContext.getMethod();
        MediaType mediaType = requestContext.getMediaType();
        InputStream inputStream = requestContext.getEntityStream();
        UriInfo uriInfo = requestContext.getUriInfo();

        WebTarget target = client.target(Main.BASE_URI).path(path);

        for (Map.Entry<String, List<String>> entry : uriInfo.getQueryParameters().entrySet()) {
            String key = entry.getKey();
            List<String> values = entry.getValue();
            for (String value : values) {
                target = target.queryParam(key, value);
            }
        }
        
        Response clientResponse;

        if (inputStream.available() > 0 && mediaType != null) {
            clientResponse = target
                .request(mediaType)
                .method(method, Entity.entity(inputStream, mediaType));
        } else {
            clientResponse = target
                .request(mediaType)
                .method(method);
        }

        final int status = clientResponse.getStatus();
        final InputStream inputStream2 = clientResponse.readEntity(InputStream.class);
            
        return Response.status(status).entity(inputStream2).type(clientResponse.getMediaType()).build();
    }    
}
