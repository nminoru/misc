package com.example;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.util.List;
import javax.ws.rs.POST;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.Consumes;
import javax.ws.rs.core.MediaType;

import org.glassfish.jersey.media.multipart.FormDataContentDisposition; 
import org.glassfish.jersey.media.multipart.FormDataParam;


/**
 * Root resource (exposed at "myresource" path)
 */
@Path("/myresource")
public class MyResource {

    /**
     * Method handling HTTP GET requests. The returned object will be sent
     * to the client as "text/plain" media type.
     *
     * @return String that will be returned as a text/plain response.
     */
    @GET
    @Produces(MediaType.TEXT_PLAIN)
    @Path("/")
    public String getIt() {
        return "Got it!";
    }

    @POST
    @Consumes(MediaType.MULTIPART_FORM_DATA)
    @Produces(MediaType.TEXT_PLAIN)
    @Path("/upload")
    public String uploadFile(@FormDataParam("file") InputStream fileStream,
                             @FormDataParam("file") FormDataContentDisposition fileDisposition) {

        if (fileDisposition == null)
            return "empty";

        try {
            BufferedReader in
                = new BufferedReader(new InputStreamReader(fileStream));

            String line;
            while ((line = in.readLine()) != null)
                System.out.println(line);
        } catch (IOException e) {
            e.printStackTrace();
        }

        return fileDisposition.getName();
    }

    public static class MyRequest {
        public String foo;
    }

    public static class MyResponse {
        public String bar;
        public String baz;
        public String[][] qux; // 多次元配列が JSON 化できることの
    }

    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    @Path("/json")
    public MyResponse json(MyRequest request) {
        MyResponse response = new MyResponse();
        
        response.bar = "bar";
        response.baz = request.foo;
        response.qux = new String[2][];
        response.qux[0] = new String[2];
        response.qux[0][0] = "abc";
        response.qux[0][1] = "def";
        response.qux[1] = new String[2];
        response.qux[1][0] = "ghi";
        response.qux[1][1] = "jkl";

        return response;
    }
}
