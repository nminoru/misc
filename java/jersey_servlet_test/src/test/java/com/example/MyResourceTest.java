package com.example;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;
import javax.ws.rs.WebApplicationException;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.GenericType;;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpPut;
import org.apache.http.client.utils.URIBuilder;
import org.apache.http.entity.FileEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;
import org.glassfish.grizzly.http.server.HttpServer;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.assertEquals;


public class MyResourceTest {

    private HttpServer server;
    private WebTarget target;

    @Before
    public void setUp() throws Exception {
        server = Main.createServer();

        server.start();

        Client c = ClientBuilder.newClient();

        target = c.target(Main.BASE_URI);
    }

    @After
    public void tearDown() throws Exception {
        server.shutdownNow();
    }

    /**
     * Test to see that the message "Got it!" is sent in the response.
     */
    @Test
    public void testGetIt() {
        String responseMsg1 = target.path("/path1/myresource/get").request().get(String.class);

        assertEquals("Got it!", responseMsg1);

        String responseMsg2 = target.path("/path2/myresource/get").request().get(String.class);

        assertEquals("Got it!", responseMsg2);
    }

    @Test
    public void testLargeUpload() throws Exception {
        System.out.println("1 GiB");
        _testLargeUpload( 1L * 1024L * 1024L * 1024L);
        System.out.println("4 GiB");
        _testLargeUpload( 4L * 1024L * 1024L * 1024L);
        // System.out.println("16 GiB");
        // _testLargeUpload(16L * 1024L * 1024L * 1024L);
    }

    void _testLargeUpload(long fileSize) throws Exception {
        File tempFile = File.createTempFile("dummy-file", "dump");

        try (RandomAccessFile fileWriter = new RandomAccessFile(tempFile, "rw")) {
            long pos = fileSize;

            if (pos > 0) {
                fileWriter.seek(pos - 1);
                fileWriter.writeChar(0);
            }
        }

        try (CloseableHttpClient httpClient = HttpClientBuilder.create().build()) {
            URI uri = URI.create(Main.BASE_URI + "/path1/myresource/large_upload_w_query?path=file");

            // HttpPut request = new HttpPut(uri);
            HttpPost request = new HttpPost(uri);
            request.setHeader("Content-Type", "application/octet-stream");

            // Request body
            FileEntity fileEntity = new FileEntity(tempFile);
            fileEntity.setChunked(true);
            request.setEntity(fileEntity);

            HttpResponse response = httpClient.execute(request);
            System.out.println("satus-code: " + response.getStatusLine().getStatusCode());
            assertEquals(HttpStatus.SC_NO_CONTENT, response.getStatusLine().getStatusCode());

        } catch (Exception e) {
            e.printStackTrace(System.err);
            throw e;
        }

        tempFile.delete();
    }
}
