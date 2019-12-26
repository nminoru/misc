package jp.nminoru.jersey_jaxb_test;

import javax.ws.rs.Consumes;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

import jp.nminoru.jersey_jaxb_test.model.Foo;


@Path("/myresource")
public class MyResource {

    @Path("/foo")
    @POST
    @Consumes(MediaType.APPLICATION_JSON)        
    @Produces(MediaType.APPLICATION_JSON)    
    public Foo postFoo(Foo request) {
        return request;
    }
}
