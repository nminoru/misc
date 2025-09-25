package jp.nminoru.jersey_jaxb_test;

import java.util.List;
import jakarta.ws.rs.Consumes;
import jakarta.ws.rs.Path;
import jakarta.ws.rs.POST;
import jakarta.ws.rs.Produces;
import jakarta.ws.rs.core.MediaType;

import jp.nminoru.jersey_jaxb_test.model.Baz;
import jp.nminoru.jersey_jaxb_test.model.Foo;
import jp.nminoru.jersey_jaxb_test.model.FooFoo;


@Path("/myresource")
public class MyResource {

    @Path("/foo")
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Foo postFoo(Foo request) {
        return request;
    }

    @Path("/foofoo")
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public FooFoo postFoo(FooFoo request) {
        return request;
    }    

    @Path("/baz")
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public Baz postBaz(Baz request) {
        if (request != null) {
            if (request.child1 != null) {
                if (request.child1.field1 != null) {
                } else {
                    System.out.println("child1.field1: null");
                }
                if (request.child1.field2 != null) {
                } else {
                    System.out.println("child1.field2: null");
                }
            } else {
                System.out.println("child1: null");
            }

            if (request.child2 != null) {
                if (request.child2.field1 != null) {
                } else {
                    System.out.println("child2.field1: null");
                }
                if (request.child2.field2 != null) {
                } else {
                    System.out.println("child2.field2: null");
                }
            } else {
                System.out.println("child2: null");
            }
        } else {
            System.out.println("body: null");
        }

        return request;
    }

    @Path("/qux")
    @POST
    @Consumes(MediaType.APPLICATION_JSON)
    @Produces(MediaType.APPLICATION_JSON)
    public List<String> postQux(List<String> request) {
        return request;
    }
}
