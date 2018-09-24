package com.example;

import java.math.BigDecimal;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
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

public class ClientTest {

    public static class Project {
        public String name;
        public int id;
        public String http_url_to_repo;
        
        public Project() {
        }

        public Project(String name, int id, String http_url_to_repo) {
            this.name = name;
            this.id = id;
            this.http_url_to_repo = http_url_to_repo;
        }
    }
        

    public static void main(String[] args) {
        ClientTest c = new ClientTest();

        c.createProject("abc");
        c.createProject("def");
        c.createProject("hij");
        c.createProject("abc");
        
        List<Project> projects = c.listProjects();
        
        for (Project project : projects) {

            System.out.println("name: " + project.name);
            System.out.println("id: " + Integer.toString(project.id));
            System.out.println("http_url_to_repo: " + project.http_url_to_repo);
            System.out.println();

            // c.listBranches(project.id);
            
            c.deleteProject(project.id);
        }
    }

    public WebTarget generateTarget() {
        Client client = ClientBuilder.newBuilder()
            .build();

        return client.target("http://127.0.0.1:80/api/v4/");
    }

    /**
     * リポジトリの一覧を表示
     */
    public List<Project> listProjects() {
        WebTarget target = generateTarget();

        Response response = target
            .path("/projects")
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .get();

        System.out.println("listProjects: " + response.getStatus());
        
        if (!response.getStatusInfo().equals(Response.Status.OK))
            return null;

        System.out.println("done");

        String content = response.readEntity(String.class);

        Jsonb jsonb = JsonbBuilder.create();

        // Type hashMapType = new HashMap<String, Object>(){}.getClass().getGenericSuperclass();
        // Type hashMapType = new HashMap<String, Object>(){}.getClass();
        // Map<String, Object> result = jsonb.fromJson(response, hashMapType);
        List repositories = jsonb.fromJson(content, List.class);

        List<Project> result = new ArrayList<>();
        for (Object object : repositories) {
            Map<String, Object> map = (Map<String, Object>) object;

            String name = (String)map.get("name");
            int id = ((BigDecimal)map.get("id")).intValue();
            String http_url_to_repo = (String)map.get("http_url_to_repo");
            
            result.add(new Project(name, id, http_url_to_repo));
        }
        
        return result;
    }

    /**
     * リポジトリの一覧を表示
     */
    public void createProject(String projectName) {
        WebTarget target = generateTarget();

        Response response = target        
            .path("/projects")
            .queryParam("name", projectName)
            .queryParam("tag_list", "project")
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .post(null);

        System.out.println("createProject: " + response.getStatus());

        if (!response.getStatusInfo().equals(Response.Status.CREATED))
            return;
        
        System.out.println("done");        
    }

    public void listBranches(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d/repository/branches", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .get();

        System.out.println("listBranches: " + response.getStatus());
        
        if (!response.getStatusInfo().equals(Response.Status.OK))
            return;
        
        System.out.println("done");        

        String content = response.readEntity(String.class);        

        Jsonb jsonb = JsonbBuilder.create();

        List branches = jsonb.fromJson(content, List.class);

        List<Project> result = new ArrayList<>();
        
        for (Object object : branches) {
            Map<String, Object> map = (Map<String, Object>) object;

            printMap(map);
        }        
    }

    /**
     * リポジトリの一覧を表示
     */
    public void deleteProject(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .delete();

        System.out.println("deleteProject: " + response.getStatus());
        
        if (!response.getStatusInfo().equals(Response.Status.ACCEPTED))
            return;

        System.out.println("done");
    }

    void printMap(Map<String, Object> map) {
        for (String key : map.keySet()) {
            System.out.print("key: " + key);
            Object value = map.get(key);
            if (value != null)
                System.out.print("  value: " + value.getClass().getName());
            System.out.println();
        }

        System.out.println();
    }
}

