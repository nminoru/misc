package com.example;

<<<<<<< HEAD
import java.lang.reflect.Type;
import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;
import java.util.List;
import javax.json.bind.Jsonb;
import javax.json.bind.JsonbBuilder;
=======
import java.util.ArrayList;
import java.util.List;
>>>>>>> 855bfdc483cf3874d848c14e48b2c688537e20ac
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.MediaType;

import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import static org.junit.Assert.*;

public class ClientTest {

    public static void main(String[] args) {
        ClientTest c = new ClientTest();

        c.testProjects();
    }

    public WebTarget generateTarget() {
        Client client = ClientBuilder.newBuilder()
            .build();

        return client.target("http://127.0.0.1:80/api/v4/");
    }

<<<<<<< HEAD
    /**
     * リポジトリの一覧を表示
     */
=======
>>>>>>> 855bfdc483cf3874d848c14e48b2c688537e20ac
    public void testProjects() {
        WebTarget target = generateTarget();

        String response = target
            .path("/projects")
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .get(String.class);
<<<<<<< HEAD

        System.out.println(response);

        Jsonb jsonb = JsonbBuilder.create();

        // Type hashMapType = new HashMap<String, Object>(){}.getClass().getGenericSuperclass();
        // Type hashMapType = new HashMap<String, Object>(){}.getClass();
        // Map<String, Object> result = jsonb.fromJson(response, hashMapType);
        List repositories = jsonb.fromJson(response, List.class);

        for (Object object : repositories) {
            Map<String, Object> map = (Map<String, Object>) object;

            for (String key : map.keySet()) {
                System.out.print("key: " + key);
                Object value = map.get(key);
                if (value != null)
                    System.out.print("  value: " + value.getClass().getName());
                System.out.println();
            }
            
            System.out.println();
        }
=======
        
        // assertTrue(response.success);

        System.out.println(response);
>>>>>>> 855bfdc483cf3874d848c14e48b2c688537e20ac
    }
}

