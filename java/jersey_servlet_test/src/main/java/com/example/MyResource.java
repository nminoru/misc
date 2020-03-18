package com.example;

import java.io.InputStream;
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
    public void uploadLargeStream1(InputStream fileStream) {
    }

    @POST
    @Consumes(MediaType.APPLICATION_OCTET_STREAM)
    @Path("/large_upload_w_query")
    public void uploadLargeStream2(@QueryParam("path") String path,
                                   InputStream fileStream) {
    }

    @PUT
    @Consumes(MediaType.APPLICATION_OCTET_STREAM)
    @Path("/large_upload_w_query")
    public void uploadLargeStream3(@QueryParam("path") String path,
                                   InputStream fileStream) {
    }
}
