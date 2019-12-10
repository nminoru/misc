package com.example;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.Consumes;
import javax.ws.rs.core.MediaType;

import org.glassfish.jersey.media.multipart.FormDataContentDisposition; 
import org.glassfish.jersey.media.multipart.FormDataParam;


@Path("/")
public class MyResource {

    @GET
    @Produces(MediaType.TEXT_PLAIN)
    @Path("/")
    public String getIt() {
        try {
            while (true) {
                System.out.println("* " + Thread.currentThread().getId());
                Thread.sleep(1000);
            }
        } catch (InterruptedException e) {
        }
        
        return "Got it!";
    }
}
