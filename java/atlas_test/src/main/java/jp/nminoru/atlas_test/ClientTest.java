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
import javax.ws.rs.client.Client;
import javax.ws.rs.client.ClientBuilder;
import javax.ws.rs.client.Entity;
import javax.ws.rs.client.WebTarget;
import javax.ws.rs.core.GenericType;;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import org.apache.commons.io.IOUtils;
import org.glassfish.jersey.client.ClientConfig;
import org.glassfish.jersey.client.ClientProperties;
import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.jsonb.JsonBindingFeature;
import org.glassfish.jersey.moxy.json.MoxyJsonFeature;

import static org.junit.Assert.*;


public class ClientTest {

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
            
        // 過去のエントリを全削除
        clientTest.deleteAllEntityOfType("test_proces");
        clientTest.deleteAllEntityOfType("test_path");
        
        // put("name",          name);
        // request.entity.attributes.put(, qualifiedName);
        //request.entity.attributes.put("path",          path);

        // エンティティの定義
        String guid1 =
            clientTest.createOrUpdateEntity("test_path",
                                            new HashMap<String, Object>() {
                                                {
                                                    put("name",          "abc");
                                                    put("qualifiedName", "abc@0");
                                                    put("path",           "/abc.txt");
                                                }
                                            });

        String guid2 =
            clientTest.createOrUpdateEntity("test_path",
                                            new HashMap<String, Object>() {
                                                {
                                                    put("name",          "def");
                                                    put("qualifiedName", "def@0");
                                                    put("path",           "/def.txt");
                                                }
                                            });

        String guid3 =
            clientTest.createOrUpdateEntity("test_path",
                                            new HashMap<String, Object>() {
                                                {
                                                    put("name",          "hij");
                                                    put("qualifiedName", "hij@0");
                                                    put("path",           "/hij.txt");
                                                }
                                            });

        String guid4 =
            clientTest.createOrUpdateEntity("test_path",
                                            new HashMap<String, Object>() {
                                                {
                                                    put("name",          "klm");
                                                    put("qualifiedName", "klm@0");
                                                    put("path",           "/klm.txt");
                                                }
                                            });        

        // プロセスの定義
        clientTest.createOrUpdateEntity("test_process",
                                        new HashMap<String, Object>() {
                                            {
                                                put("name",          "test1");
                                                put("qualifiedName", "test1");
                                                put("description",   "DESCRIPTION");
                                                put("inputs",
                                                    new ArrayList() {
                                                        {
                                                            // add(getDatasetWithQualifiedName("abc@0"));
                                                            add(getDatasetWithGuid(guid1));
                                                        }
                                                    });

                                                put("outputs",
                                                    new ArrayList() {
                                                        {
                                                            // add(getDatasetWithQualifiedName("def@0"));
                                                            add(getDatasetWithGuid(guid2));
                                                        }
                                                    });                                                    
                                            }
                                        });

        clientTest.createOrUpdateEntity("test_process",
                                        new HashMap<String, Object>() {
                                            {
                                                put("name",          "test2");
                                                put("qualifiedName", "test2");
                                                put("description",   "DESCRIPTION");
                                                put("inputs",
                                                    new ArrayList() {
                                                        {
                                                            // add(getDatasetWithQualifiedName("def@0"));
                                                            add(getDatasetWithGuid(guid2));
                                                        }
                                                    });

                                                put("outputs",
                                                    new ArrayList() {
                                                        {
                                                            // add(getDatasetWithQualifiedName("hij@0"));
                                                            add(getDatasetWithGuid(guid3));
                                                        }
                                                    });                                                    
                                            }
                                        });

        clientTest.createOrUpdateEntity("test_process",
                                        new HashMap<String, Object>() {
                                            {
                                                put("name",          "test1");
                                                put("qualifiedName", "test1");
                                                put("description",   "DESCRIPTION");
                                                put("inputs",
                                                    new ArrayList() {
                                                        {
                                                            // add(getDatasetWithQualifiedName("klm@0"));
                                                            add(getDatasetWithGuid(guid4));
                                                        }
                                                    });

                                                put("outputs",
                                                    new ArrayList() {
                                                        {
                                                            // add(getDatasetWithQualifiedName("def@0"));
                                                            add(getDatasetWithGuid(guid2));
                                                        }
                                                    });                                                    
                                            }
                                        });        
    }

    static Map<String, Object> getDatasetWithQualifiedName(String qualifiedName) {
        return new HashMap<String, Object>() {
            {
                put("typeName", "test_path");
                put("uniqueAttributes",
                    new HashMap<String, Object>() {
                        {
                            put("qualifiedName", qualifiedName);
                        }
                    });
            }
        };
    }

    static Map<String, Object> getDatasetWithGuid(String guid) {
        return new HashMap<String, Object>() {
            {
                put("typeName", "test_path");
                put("guid",     guid);
            }
        };
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
            
            // JSON バインディングを選択する
            // .register(JsonBindingFeature.class)
            // .register(MoxyJsonFeature.class)
            .register(JacksonFeature.class)
            ;

        return client.target("http://127.0.0.1:21000/api/atlas");
    }

    public String createOrUpdateEntity(String typeName, Map<String, Object> attributes) {
        AtlasEntityWithExtInfo request = new AtlasEntityWithExtInfo();
        
        request.entity.typeName = typeName;
        request.entity.attributes = attributes;

        EntityMutationResponse response = createOrUpdateEntity(request);
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
                    return entityHeader.guid;
                }
            }
            
            if (entityHeaderList2 != null) {
                for (AtlasEntityHeader entityHeader: entityHeaderList2) {
                    return entityHeader.guid;
                }
            }            
        }

        return null;
    }

    public EntityMutationResponse createOrUpdateEntity(AtlasEntityWithExtInfo request) {
        WebTarget target = generateTarget();
        
        Response response = target        
            .path("/v2/entity")
            .request(MediaType.APPLICATION_JSON)
            .post(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));
        
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL)) {
            System.err.println("status: " + response.getStatus());
            System.err.println("response: " + response.readEntity(String.class));
            return null;
        }
        
        return response.readEntity(EntityMutationResponse.class);
    }

    public void deleteAllEntityOfType(String typeName) {
        AtlasSearchResult searchResult = searchDSL(typeName);
        
        if (searchResult != null && searchResult.entities != null) {
            for (AtlasEntityHeader entity : searchResult.entities) {
                String guid = entity.guid;

                System.out.println(entity.guid + " " + entity.status + " " + entity.displayText);
                                   
                deleteEntity(guid);
                //AtlasEntityWithExtInfo entityWithExtInfo = clientTest.getEntity(guid);
                // List<String> superTypes = clientTest.getSuperTypes(entity.entity.typeName);
                // AtlasLineageInfo lineage = clientTest.getLineage(guid);                
            }
        }
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
        
        return response.readEntity(AtlasTypesDef.class);
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

    public AtlasSearchResult searchDSL(String typeName) {
        WebTarget target = generateTarget();

        Response response = target
            .path("/v2/search/dsl")
            .queryParam("query", "where __state = \"ACTIVE\"")
            .queryParam("typeName", typeName)
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
        
        return response.readEntity(new GenericType<List<AtlasGlossary>>(){});
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
