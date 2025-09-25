package jp.nminoru.jersey_jaxb_test;

import java.util.List;
import jakarta.ws.rs.WebApplicationException;
import jakarta.ws.rs.client.Client;
import jakarta.ws.rs.client.ClientBuilder;
import jakarta.ws.rs.client.Entity;
import jakarta.ws.rs.client.WebTarget;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;
import org.glassfish.grizzly.http.server.HttpServer;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import jp.nminoru.jersey_jaxb_test.model.EnumType0;
import jp.nminoru.jersey_jaxb_test.model.EnumType1;
import jp.nminoru.jersey_jaxb_test.model.Foo;


public class MyResourceTest {

    protected HttpServer server;
    protected WebTarget  target;

    protected void setUp(Class feature) throws Exception {
        server = Main.startServer(feature);

        Client client = ClientBuilder.newBuilder()
            .register(feature)
            .build();

        target = client.target(Main.BASE_URI);
    }

    protected void tearDown() throws Exception {
        server.shutdownNow();
    }

    protected void test1_1() throws Exception {
        System.out.println("--- JsonBindingTest: STEP 1-1 -----------");

        String request = """
	    {"enumType0":"abc","version0":2,"version1":2, "options0":{"key1":"value1"}, "options1":{"key1":"value1"}, "options2":{"key1":"value1"}}""";

        Response response = target
            .path("/myresource/foo")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response.readEntity(String.class));
            assertTrue(false);
        }

        System.out.println("-------------");

        Foo result = response.readEntity(Foo.class);

        assertNotNull(result);
        assertEquals(EnumType0.ABC,         result.enumType0);
        assertEquals(EnumType1.unspecified, result.enumType1);
        assertEquals(1,                     result.int0);
        assertEquals(Integer.valueOf(1),    result.int1);
        assertEquals(Integer.valueOf(2),    result.version0);
        assertEquals(Integer.valueOf(2),    result.version1);

        assertNotNull(result.options0);
        // assertEquals("value1",       result.options0.get("key1"));

        assertNotNull(result.options1);
        // assertEquals("value1",       result.options1._key1);

        assertNotNull(result.options2);
        assertNull(result.options2.key1); // fill できない
        assertEquals("value1",       result.options2.get("key1"));

        System.out.println("-------------");
    }

    protected void test1_2() throws Exception {
        System.out.println("--- JsonBindingTest: STEP 1- -----------");

        String request = """
	    {"enumType0":"abc","version0":2,"version1":2, "options0":{"key1":"value1"}, "options1":{"key1":"value1"}, "options2":{"key1":"value1"}}""";

        Response response = target
            .path("/myresource/foofoo")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response.readEntity(String.class));
            assertTrue(false);
        }

        System.out.println("-------------");

        Foo result = response.readEntity(Foo.class);

        assertNotNull(result);
        assertEquals(EnumType0.ABC,         result.enumType0);
        assertEquals(EnumType1.unspecified, result.enumType1);
        assertEquals(1,                     result.int0);
        assertEquals(Integer.valueOf(1),    result.int1);
        assertEquals(Integer.valueOf(2),    result.version0);
        assertEquals(Integer.valueOf(2),    result.version1);

        assertNotNull(result.options0);
        // assertEquals("value1",       result.options0.get("key1"));

        assertNotNull(result.options1);
        // assertEquals("value1",       result.options1._key1);

        assertNotNull(result.options2);
        assertNull(result.options2.key1); // fill できない
        assertEquals("value1",       result.options2.get("key1"));

        System.out.println("-------------");
    }

    protected void test2() throws Exception {
        String request = """
	    {"transperent0":10, "transperent1":11}""";

        Response response = target
            .path("/myresource/foo")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response.readEntity(String.class));
            assertTrue(false);
        }

        System.out.println("result: " + response.readEntity(String.class));
    }

    protected void test3() throws Exception {
        String request1 = "{}";

        Response response1 = target
            .path("/myresource/baz")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request1, MediaType.APPLICATION_JSON_TYPE));

        if (!response1.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response1.readEntity(String.class));
            assertTrue(false);
        }

        String request2 = """
	    {"child1":{},"child2":null}""";

        Response response2 = target
            .path("/myresource/baz")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request2, MediaType.APPLICATION_JSON_TYPE));

        if (!response2.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response2.readEntity(String.class));
            assertTrue(false);
        }
    }

    protected void test4() throws Exception {
        String request1 = """
	    ["ABC", "EFG", "HIJ"]""";

        Response response1 = target
            .path("/myresource/qux")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request1, MediaType.APPLICATION_JSON_TYPE));

        if (!response1.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response1.readEntity(String.class));
            assertTrue(false);
        }

        System.err.println("result: " + response1.readEntity(String.class));
    }
}
