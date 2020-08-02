package jp.nminoru.jersey_jaxb_test;

import javax.ws.rs.client.Entity;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import jp.nminoru.jersey_jaxb_test.model.EnumType0;
import jp.nminoru.jersey_jaxb_test.model.EnumType1;
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
    public void test1() throws Exception {
        System.out.println("--- JacksonTest: STEP 1 -----------");

        super.test1();
    }

    @Test
    public void test2() throws Exception {
        System.out.println("--- JacksonTest: STEP 2 -----------");

        super.test2();
    }

    @Test
    public void test3() throws Exception {
        System.out.println("--- JacksonTest: STEP 3 -----------");

        super.test3();
    }

    @Test
    public void test4() throws Exception {
        System.out.println("--- JacksonTest: STEP 4 -----------");

        super.test4();
    }
}
