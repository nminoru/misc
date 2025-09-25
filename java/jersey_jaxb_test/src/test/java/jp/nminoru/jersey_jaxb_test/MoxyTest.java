package jp.nminoru.jersey_jaxb_test;

import jakarta.ws.rs.client.Entity;
import jakarta.ws.rs.core.MediaType;
import jakarta.ws.rs.core.Response;
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
	System.out.println("-------------------------------------------------------");
	System.out.println("  Moxy Test                                            ");
	System.out.println("-------------------------------------------------------");

        super.setUp(MoxyJsonFeature.class);
    }

    @After
    public void tearDown() throws Exception {
        super.tearDown();
    }

    @Test
    public void test1_1() throws Exception {
        System.out.println("--- MoxyTest: STEP 1-1 -----------");

        super.test1_1();
    }

    @Test
    public void test1_2() throws Exception {
        System.out.println("--- MoxyTest: STEP 1-2 -----------");

        super.test1_2();
    }

    @Test
    public void test2() throws Exception {
        System.out.println("--- MoxyTest: STEP 2 -----------");

        super.test2();
    }

    @Test
    public void test3() throws Exception {
        System.out.println("--- MoxyTest: STEP 3 -----------");

        super.test3();
    }

    @Test
    public void test4() throws Exception {
        System.out.println("--- MoxyTest: STEP 4 -----------");

        super.test4();
    }
}
