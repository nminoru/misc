package com.example;

import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.Consumes;
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
}
