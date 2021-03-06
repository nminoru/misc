package com.example;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.PUT;
import javax.ws.rs.QueryParam;
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

    @POST
    @Consumes(MediaType.APPLICATION_OCTET_STREAM)
    @Path("/large_upload")
    public void uploadLargeStream1(InputStream fileStream) throws IOException {
        uploadLargeStream(fileStream);
    }

    @POST
    @Consumes(MediaType.APPLICATION_OCTET_STREAM)
    @Path("/large_upload_w_query")
    public void uploadLargeStream2(@QueryParam("path") String path, InputStream fileStream) throws IOException {
        uploadLargeStream(fileStream);
    }

    @PUT
    @Consumes(MediaType.APPLICATION_OCTET_STREAM)
    @Path("/large_upload_w_query")
    public void uploadLargeStream3(@QueryParam("path") String path,
                                   InputStream fileStream) throws IOException {
        uploadLargeStream(fileStream);
    }

    void uploadLargeStream(InputStream fileStream) throws IOException {
        byte[] bytes = new byte[4096];
        int  ret = 0;
        long pos = 0;
        long reportedPos = 0;

        while ((ret = fileStream.read(bytes)) > 0) {
            while (reportedPos + 16L * 1024L * 1024L < pos) {
                System.out.println("recv: " + pos);
                reportedPos += 16L * 1024L * 1024L;
            }

            pos += ret;
        }
    }

    public static class MyRequest {
        public String foo;
    }

    public static class MyResponse {
        public String bar;
        public String baz;
        public String[][] qux; // 多次元配列が JSON 化できることの
    }

    public static class Type {
        public String name;
        public String value;

        public Type() {
        }

        public Type(String name, String value) {
            this.name = name;
            this.value = value;
        }
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

    @GET
    @Produces(MediaType.APPLICATION_JSON)
    @Path("/jsonarray")
    public List<Type> jsonarray() {
        return new ArrayList<Type>() {
            {
                add(new Type("NAME1", "VALUE1"));
                add(new Type("NAME2", "VALUE2"));
                add(new Type("NAME3", "VALUE3"));
            }
        };
    }

    @GET
    @Consumes(MediaType.TEXT_PLAIN)
    @Path("/content")
    public String getContent1() {
        return "text/plain";
    }

    @GET
    @Consumes(MediaType.APPLICATION_JSON)
    @Path("/content")
    public String getContent2() {
        return "application/json";
    }
}
