package jp.nminoru.atlas_test;

import java.io.InputStream;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
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
import org.apache.commons.io.IOUtils;
import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.ClientProperties;
import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.moxy.json.MoxyJsonFeature;

import static org.junit.Assert.*;

public class ClientTest {

    Jsonb jsonb;
    
    public static void main(String[] args) throws IOException {
        ClientTest clientTest = new ClientTest();

        clientTest.install();

        //
        List<AtlasGlossary> glossaries = clientTest.getGlossaries();
        for (AtlasGlossary glossary : glossaries) {
            System.out.println("guid:          " + glossary.guid);
            System.out.println("qualifiedName: " + glossary.qualifiedName);
            System.out.println("name:          " + glossary.name);
            System.out.println();
        }
            
        //
        AtlasSearchResult searchResult = clientTest.searchDSL();
        if (searchResult != null && searchResult.entities != null) {
            for (AtlasEntityHeader entity : searchResult.entities) {
                String guid = entity.guid;
                AtlasEntityWithExtInfo entityWithExtInfo = clientTest.getEntity(guid);
                
                // List<String> superTypes = clientTest.getSuperTypes(entity.entity.typeName);
                // AtlasLineageInfo lineage = clientTest.getLineage(guid);                
            }
        }

        //
        AtlasEntityWithExtInfo request = new AtlasEntityWithExtInfo();
        request.entity.typeName = "test_path";
        request.entity.attributes.put("name",          "abc"     );
        request.entity.attributes.put("qualifiedName", "abc@abc" );
        request.entity.attributes.put("path",          "/abc.txt");

        EntityMutationResponse response = clientTest.createEntity(request);
        // EntityMutationResponse response = clientTest.createEntity(request);
        
        assert response != null;
        assert response.mutatedEntities != null;        

        if (response.mutatedEntities != null) {
            List<AtlasEntityHeader> entityHeaderList1;
            List<AtlasEntityHeader> entityHeaderList2;
            
            entityHeaderList1 = (List<AtlasEntityHeader>)response.mutatedEntities.get("CREATE");
            entityHeaderList2 = (List<AtlasEntityHeader>)response.mutatedEntities.get("UPDATE");
            
            if (entityHeaderList1 != null) {
                for (AtlasEntityHeader entityHeader: entityHeaderList1) {
                    ;
                }
            }
            
            if (entityHeaderList2 != null) {
                for (AtlasEntityHeader entityHeader: entityHeaderList2) {
                    ;
                }
            }            
        }
    }

    public void install() throws IOException {
        AtlasBaseTypeDef typeDef = getTypeDefByName("test_path");

        if (typeDef != null)
            return;
            
        System.out.println("loading initial data to Apache Atlas");
        
        try (InputStream inputStream = getClass().getResourceAsStream("typedef-entities.json")) {
            String content = IOUtils.toString(inputStream, Charset.defaultCharset());

            createAtlasTypeDef(content);
        }
    }

    public ClientTest() {
        jsonb = JsonbBuilder.create();
    }

    public WebTarget generateTarget() {
        HttpAuthenticationFeature feature =
            HttpAuthenticationFeature.basic("admin", "admin");

        // Apache Atlas がリクエストボディ有りの DELETE メソッドを使うので
        ClientConfig config = new ClientConfig();
        config.property(ClientProperties.SUPPRESS_HTTP_COMPLIANCE_VALIDATION, true);
        
        Client client = ClientBuilder.newBuilder()
            .newClient(config);
        
        client
            .register(feature)
            // .register(MoxyJsonFeature.class);
            .register(JacksonFeature.class);

        return client.target("http://127.0.0.1:21000/api/atlas");
    }

    public EntityMutationResponse createEntity(AtlasEntityWithExtInfo request) {
        WebTarget target = generateTarget();

        String res = jsonb.toJson(request);        
        
        Response response = target        
            .path("/v2/entity")
            .request(MediaType.APPLICATION_JSON)
            // .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));
            .post(Entity.entity(res, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.out.println("status: " + response.getStatus());
            System.out.println("response: " + response.readEntity(String.class));
            return null;
        }

        return response.readEntity(EntityMutationResponse.class);
    }

    public EntityMutationResponse deleteEntity(String guid) {
        WebTarget target = generateTarget();
        
        Response response = target        
            .path("/v2/entity/guid/" + guid)
            .request(MediaType.APPLICATION_JSON)
            .delete();
        
        return response.readEntity(EntityMutationResponse.class);
    }

    public AtlasEntityWithExtInfo getEntity(String guid) {
        WebTarget target = generateTarget();
        
        AtlasEntityWithExtInfo response = target
            .path("/v2/entity/guid/" + guid)
            .request(MediaType.APPLICATION_JSON)
            .get(AtlasEntityWithExtInfo.class);

        return response;
    }

    public AtlasTypesDef createAtlasTypeDef(String request) {
        WebTarget target = generateTarget();
        
        Response response = target
            .path("/v2/types/typedefs")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));            

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;

        String content = response.readEntity(String.class);
        
        System.out.println(content);
        
        AtlasTypesDef result =
            jsonb.fromJson(content, AtlasTypesDef.class);

        return result;
    }

    public AtlasBaseTypeDef getTypeDefByName(String name) {
        WebTarget target = generateTarget();
        
        Response response = target
            .path("/v2/types/typedef/name/" + name)
            .request(MediaType.APPLICATION_JSON)            
            .get();

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;

        return response.readEntity(AtlasBaseTypeDef.class);
    }

    public AtlasSearchResult searchDSL() {
        WebTarget target = generateTarget();

        Response response = target
            .path("/v2/search/dsl")
            .queryParam("typeName", "test_path")
            .request(MediaType.APPLICATION_JSON)
            .get();
        
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;        

        return response.readEntity(AtlasSearchResult.class); 
    }

    public AtlasSearchResult searchDSL2() {
        WebTarget target = generateTarget();

        Response response = target        
            .path("/v2/search/dsl")
            .queryParam("typeName", "test_path")
            .queryParam("query", "WHERE title = FOO")
            .request(MediaType.APPLICATION_JSON)
            .get();

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;        

        return response.readEntity(AtlasSearchResult.class); 
    }

    public AtlasLineageInfo getLineage(String guid) {
        WebTarget target = generateTarget();
        
        Response response = target
            .path("/v2/lineage/" + guid)
            .queryParam("direction", "BOTH")
            .queryParam("depth", "3")
            .request(MediaType.APPLICATION_JSON)
            .get();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        return response.readEntity(AtlasLineageInfo.class);
    }

    public AtlasEntitiesWithExtInfo getBulk(List<String> guids) {
        WebTarget target = generateTarget();
        
        target = target.path("/v2/entity/bulk");

        for (String guid : guids)
            target.queryParam("guid", guid);
        
        Response response = target
            .request(MediaType.APPLICATION_JSON)
            .get();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        return response.readEntity(AtlasEntitiesWithExtInfo.class);
    }

    public EntityMutationResponse deleteBulk(List<String> guids) {
        WebTarget target = generateTarget();
        
        target = target.path("/v2/entity/bulk");

        for (String guid : guids)
            target.queryParam("guid", guid);
        
        Response response = target
            .request(MediaType.APPLICATION_JSON)            
            .delete();

        assertTrue(response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL));

        return response.readEntity(EntityMutationResponse.class);
    }

    public List<String> getSuperTypes(String name) {
        Set<String> superTypes = new HashSet<>();

        _getSuperTypes(name, superTypes);

        return superTypes.stream().collect(Collectors.toList());
    }

    public void _getSuperTypes(String name, Set<String> superTypes) {
        AtlasBaseTypeDef typeDef = getTypeDefByName(name);
        
        superTypes.add(typeDef.name);

        for (String superType : typeDef.superTypes) {
            _getSuperTypes(superType, superTypes);
        }
    }

    public List<AtlasGlossary> getGlossaries() {
        WebTarget target = generateTarget();
        
        Response response = target
            .path("/v2/glossary")
            .request(MediaType.APPLICATION_JSON)            
            .get();

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;

        String content = response.readEntity(String.class);
        List<AtlasGlossary> result =
            jsonb.fromJson(content,
                           new ArrayList<AtlasGlossary>(){}.getClass().getGenericSuperclass());
        
        return result;
    }

    public AtlasGlossary createGlossary(AtlasGlossary request) {
        WebTarget target = generateTarget();
        
        Response response = target
            .path("/v2/glossary")
            .request(MediaType.APPLICATION_JSON)            
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));

        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;        

        return response.readEntity(AtlasGlossary.class);
    }    
}
