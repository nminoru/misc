package jp.nminoru.jersey_jaxb_test;

import java.util.List;
import javax.ws.rs.Consumes;
import javax.ws.rs.Path;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

import jp.nminoru.jersey_jaxb_test.model.Baz;
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
