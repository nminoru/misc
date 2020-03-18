package com.example;

import java.io.InputStream;
import java.io.IOException;
import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.Consumes;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.PUT;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;



@Path("/myresource")
public class MyResource {
    @Context
    private HttpServletRequest request;

    @GET
    @Produces(MediaType.TEXT_PLAIN)
    @Path("/get")
    public String getIt() {
        HttpSession session = request.getSession(true);

        return "Got it!";
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
    public void uploadLargeStream3(@QueryParam("path") String path, InputStream fileStream) throws IOException {
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
}
