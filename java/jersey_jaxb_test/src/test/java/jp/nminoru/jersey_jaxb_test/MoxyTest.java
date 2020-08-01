package jp.nminoru.jersey_jaxb_test;

import javax.ws.rs.client.Entity;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.glassfish.jersey.moxy.json.MoxyJsonFeature;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import jp.nminoru.jersey_jaxb_test.model.EnumType;
import jp.nminoru.jersey_jaxb_test.model.Foo;


public class MoxyTest extends MyResourceTest {

    @Before
    public void setUp() throws Exception {
        super.setUp(MoxyJsonFeature.class);
    }

    @After
    public void tearDown() throws Exception {
        super.tearDown();
    }

    @Test
    public void test() throws Exception {
        String request = "{\"enumType\":\"abc\",\"version0\":2,\"version1\":2, \"options0\":{\"key1\":\"value1\"}}";

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

        assertEquals(EnumType.ABC,   result.enumType);
        assertEquals(new Integer(2), result.version0);
        assertEquals(new Integer(2), result.version1);
        assertNotNull(result.options0);
        assertEquals("value1",       result.options0.get("key1"));
    }
}
