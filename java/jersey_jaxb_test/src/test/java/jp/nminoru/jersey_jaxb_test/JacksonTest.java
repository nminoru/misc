package jp.nminoru.jersey_jaxb_test;

import javax.ws.rs.client.Entity;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import jp.nminoru.jersey_jaxb_test.model.EnumType;
import jp.nminoru.jersey_jaxb_test.model.Foo;


public class JacksonTest extends MyResourceTest {

    @Before
    public void setUp() throws Exception {
        super.setUp(JacksonFeature.class);
    }

    @After
    public void tearDown() throws Exception {
        super.tearDown();
    }

    @Test
    public void test() throws Exception {
        Foo request = new Foo();

        request.enumType = EnumType.ABC;
        request.version0 = 2;
        request.version1 = 2;

        Response response = target
            .path("/myresource/foo")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response.readEntity(String.class));
            assertTrue(false);
        }

        Foo result = response.readEntity(Foo.class);

        assertNotNull(result);

        assertEquals(request.enumType, result.enumType);

        // request.version0 は null になっている
        assertNull(result.version0);

        // request.version1 は正しい
        assertEquals(request.version1, result.version1);
    }
}
