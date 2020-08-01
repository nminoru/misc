package jp.nminoru.jersey_jaxb_test;

import javax.ws.rs.client.Entity;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.glassfish.jersey.moxy.json.MoxyJsonFeature;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import jp.nminoru.jersey_jaxb_test.model.EnumType0;
import jp.nminoru.jersey_jaxb_test.model.EnumType1;
import jp.nminoru.jersey_jaxb_test.model.Foo;


public class MoxyTest extends MyResourceTest {

    @Before
    public void setUp() throws Exception {
        System.out.println("--- MoxyTest: STEP 1 -----------");
        super.setUp(MoxyJsonFeature.class);
    }

    @After
    public void tearDown() throws Exception {
        super.tearDown();
    }

    @Test
    public void test() throws Exception {
        String request = "{\"enumType0\":\"abc\",\"version0\":2,\"version1\":2, \"options0\":{\"key1\":\"value1\"}, \"options1\":{\"key1\":\"value1\"}}";

        Response response = target
            .path("/myresource/foo")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println(response.readEntity(String.class));
            assertTrue(false);
        }

        System.out.println("--- MoxyTest: STEP 2 -----------");

        Foo result = response.readEntity(Foo.class);

        assertNotNull(result);
        assertEquals(EnumType0.ABC,         result.enumType0);
        assertEquals(EnumType1.unspecified, result.enumType1);
        assertEquals(1,                     result.int0);
        assertEquals(new Integer(1),        result.int1);
        assertEquals(new Integer(2),        result.version0);
        assertEquals(new Integer(2),        result.version1);

        // Moxy では対応は対応できない
        // assertNotNull(result.options0);
        // assertEquals("value1",       result.options0.get("key1"));

        assertNotNull(result.options1);
        assertEquals("value1",       result.options1._key1);

        System.out.println("--- MoxyTest: STEP 3 -----------");        
    }
}
