package com.example;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;
import javax.json.bind.Jsonb;
import javax.json.bind.JsonbBuilder;
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import static org.junit.Assert.*;

import com.example.AtlasBaseTypeDef;
import com.example.AtlasEntityExtInfo;
import com.example.AtlasEntityHeader;
import com.example.AtlasEntityWithExtInfo;
import com.example.AtlasEntitiesWithExtInfo;
import com.example.AtlasLineageInfo;
import com.example.AtlasSearchResult;
import com.example.EntityMutationResponse;

public class ClientTest {

    public static void main(String[] args) {
        ClientTest test = new ClientTest();

        List<String> result = test.testSearchDSL();
        
        for(String guid: result) {
            AtlasEntityWithExtInfo entity = test.testGetEntity(guid);
            System.out.println(entity.entity.guid);

            List<String> superTypes = test.testGetSuperTypes(entity.entity.typeName);
            AtlasLineageInfo lineage = test.testGetLineage(guid);
        }
    }

    public WebTarget generateTarget() {
        HttpAuthenticationFeature feature = HttpAuthenticationFeature.basic("admin", "admin");
        
        Client client = ClientBuilder.newBuilder()
            .build();

        client.register(feature);

        return client.target("http://127.0.0.1:21000/api/atlas");
    }

    public EntityMutationResponse testCreateEntity(AtlasEntityWithExtInfo request) {
        WebTarget target = generateTarget();

        Response response = target        
            .path("/v2/entity")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        String content = response.readEntity(String.class);

        System.out.println(content);

        Jsonb jsonb = JsonbBuilder.create();

        EntityMutationResponse result =
            jsonb.fromJson(content, EntityMutationResponse.class);
        
        return result;
    }

    public EntityMutationResponse testDeleteEntity(String guid) {
        WebTarget target = generateTarget();

        Response response = target        
            .path("/v2/entity/guid/" + guid)
            .request(MediaType.APPLICATION_JSON)
            .delete();
        
        String content = response.readEntity(String.class);

        System.out.println(content);

        Jsonb jsonb = JsonbBuilder.create();

        EntityMutationResponse result = 
            jsonb.fromJson(content, EntityMutationResponse.class);
        
        return result;
    }

    public AtlasEntityWithExtInfo testGetEntity(String guid) {
        WebTarget target = generateTarget();

        AtlasEntityWithExtInfo response = target
            .path("/v2/entity/guid/" + guid)
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

    public List<String> testSearchDSL2() {
        WebTarget target = generateTarget();

        AtlasSearchResult response = target
            .path("/v2/search/dsl")
            .queryParam("typeName", "test_path")
            .queryParam("query", "WHERE title = FOO")
            .request(MediaType.APPLICATION_JSON)
            .get(AtlasSearchResult.class);
        
        // assertTrue(response.success);

        List<String> result = new ArrayList<>();
        
        for (AtlasEntityHeader entity : response.entities)
            result.add(entity.guid);

        return result;
    }

    public AtlasLineageInfo testGetLineage(String guid) {
        WebTarget target = generateTarget();

        Response response = target
            .path("/v2/lineage/" + guid)
            .queryParam("direction", "BOTH")
            .queryParam("depth", "3")
            .request(MediaType.APPLICATION_JSON)
            .get();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        String content = response.readEntity(String.class);

        System.out.println(content);

        Jsonb jsonb = JsonbBuilder.create();

        AtlasLineageInfo result =
            jsonb.fromJson(content, AtlasLineageInfo.class);

        return result;
    }

    public AtlasEntitiesWithExtInfo testGetBulk(List<String> guids) {
        WebTarget target = generateTarget();

        target = target.path("/v2/entity/bulk");

        for (String guid : guids)
            target.queryParam("guid", guid);
        
        Response response = target
            .request(MediaType.APPLICATION_JSON)
            .get();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        String content = response.readEntity(String.class);

        System.out.println(content);

        Jsonb jsonb = JsonbBuilder.create();

        AtlasEntitiesWithExtInfo result =
            jsonb.fromJson(content, AtlasEntitiesWithExtInfo.class);

        return result;
    }

    public EntityMutationResponse testDeleteBulk(List<String> guids) {
        WebTarget target = generateTarget();

        target = target.path("/v2/entity/bulk");

        for (String guid : guids)
            target.queryParam("guid", guid);
        
        Response response = target
            .request(MediaType.APPLICATION_JSON)            
            .delete();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        String content = response.readEntity(String.class);

        System.out.println(content);

        Jsonb jsonb = JsonbBuilder.create();

        EntityMutationResponse result =
            jsonb.fromJson(content, EntityMutationResponse.class);

        return result;
    }

    public List<String> testGetSuperTypes(String name) {
        Set<String> superTypes = new HashSet<>();

        _testGetSuperTypes(name, superTypes);

        return superTypes.stream().collect(Collectors.toList());
    }

    public void _testGetSuperTypes(String name, Set<String> superTypes) {
        AtlasBaseTypeDef typeDef = testGetTypeDefByName(name);
        
        superTypes.add(typeDef.name);

        for (String superType : typeDef.superTypes) {
            _testGetSuperTypes(superType, superTypes);
        }
    }

    public AtlasBaseTypeDef testGetTypeDefByName(String name) {
        WebTarget target = generateTarget();
        
        Response response = target
            .path("/v2/types/typedef/name/" + name)
            .request(MediaType.APPLICATION_JSON)            
            .get();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        String content = response.readEntity(String.class);

        Jsonb jsonb = JsonbBuilder.create();

        AtlasBaseTypeDef result =
            jsonb.fromJson(content, AtlasBaseTypeDef.class);

        return result;        
    }
}
