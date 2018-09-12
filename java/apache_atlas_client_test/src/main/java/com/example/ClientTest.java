package com.example;

import java.util.ArrayList;
import java.util.List;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.MediaType;

import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import static org.junit.Assert.*;

import com.example.AtlasEntityExtInfo;
import com.example.AtlasSearchResult;
import com.example.AtlasEntityHeader;
import com.example.AtlasEntityWithExtInfo;

public class ClientTest {

    public static void main(String[] args) {
        ClientTest test = new ClientTest();

        List<String> result = test.testSearchDSL();
        for(String guid: result) {
            AtlasEntityWithExtInfo entity = test.testGetEntity(guid);
            System.out.println(entity.entity.guid);
        }
    }

    public WebTarget generateTarget() {
        HttpAuthenticationFeature feature = HttpAuthenticationFeature.basic("admin", "admin");
        
        Client client = ClientBuilder.newBuilder()
            .build();

        client.register(feature);

        return client.target("http://127.0.0.1:21000/api/atlas");
    }

    public AtlasEntityWithExtInfo testGetEntity(String guid) {
        WebTarget target = generateTarget();

        AtlasEntityWithExtInfo response = target
            .path(String.format("/v2/entity/guid/%s", guid))
            .request(MediaType.APPLICATION_JSON)
            .get(AtlasEntityWithExtInfo.class);

        return response;
    }

    public List<String> testSearchDSL() {
        WebTarget target = generateTarget();

        AtlasSearchResult response = target
            .path("/v2/search/dsl")
            .queryParam("typeName", "test_path")
            .request(MediaType.APPLICATION_JSON)
            .get(AtlasSearchResult.class);
        
        // assertTrue(response.success);

        List<String> result = new ArrayList<>();
        
        for (AtlasEntityHeader entity : response.entities)
            result.add(entity.guid);

        return result;
    }    
}

