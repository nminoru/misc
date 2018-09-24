package com.example;

import java.math.BigDecimal;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.json.bind.Jsonb;
import javax.json.bind.JsonbBuilder;
import javax.json.bind.annotation.JsonbProperty;
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
        public String description;
        public List tag_list;
        
        public Project() {
        }

        public Project(String name, int id, String http_url_to_repo, String description, List tag_list) {
            this.name = name;
            this.id = id;
            this.http_url_to_repo = http_url_to_repo;
            this.description = description;
            this.tag_list = tag_list;
        }
    }

    public static class Branch {
        public String name;
        public Commit commit;

        @JsonbProperty(value = "protected")
        public boolean _protected;
        
        public Branch() {
        }

        public Branch(String name, Commit commit) {
            this.name = name;
            this.commit = commit;
        }
    }

    public static class Commit {
        public String id;
        public String short_id;
        public String message;
        public String title;
        public String committed_date;
        
        public Commit() {
        }

        public Commit(String id, String short_id, String message, String title, String committed_date) {
            this.id = id;
            this.short_id = short_id;
            this.message = message;
            this.title = title;
            this.committed_date = committed_date;
        }
    }    

    public static void main(String[] args) {
        ClientTest c = new ClientTest();

        c.createProject("abc");
        c.createProject("def");
        c.createProject("hij");
        
        List<Project> projects = c.listProjects();
        
        for (Project project : projects) {

            System.out.println("name: " + project.name);
            System.out.println("id: " + Integer.toString(project.id));
            System.out.println("http_url_to_repo: " + project.http_url_to_repo);
            System.out.println();

            c.listBranches(project.id);
            c.listCommits(project.id);
            
            // c.deleteProject(project.id);
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

        List<Project> projectList =
            jsonb.fromJson(content,
                           new ArrayList<Project>(){}.getClass().getGenericSuperclass());
        
        return projectList;
    }

    /**
     * リポジトリの一覧を表示
     */
    public Project createProject(String projectName) {
        WebTarget target = generateTarget();

        Response response = target        
            .path("/projects")
            .queryParam("name", projectName)
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .post(null);

        System.out.println("createProject: " + response.getStatus());

        if (!response.getStatusInfo().equals(Response.Status.CREATED))
            return null;

        System.out.println("done");

        String content = response.readEntity(String.class);

        Jsonb jsonb = JsonbBuilder.create();

        Project project = jsonb.fromJson(content, Project.class);

        return project;
    }

    public List<Branch> listBranches(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d/repository/branches", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .get();

        System.out.println("listBranches: " + response.getStatus());
        
        if (!response.getStatusInfo().equals(Response.Status.OK))
            return null;
        
        System.out.println("done");        

        String content = response.readEntity(String.class);        

        Jsonb jsonb = JsonbBuilder.create();

        List<Branch> branchList =
            jsonb.fromJson(content,
                           new ArrayList<Branch>(){}.getClass().getGenericSuperclass());
        
        return branchList;
    }

    public List<Commit> listCommits(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d/repository/commits", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", "qGm5VgrCzA6PpqstUXtb")
            .get();

        System.out.println("listCommits: " + response.getStatus());
        
        if (!response.getStatusInfo().equals(Response.Status.OK))
            return null;
        
        System.out.println("done");

        String content = response.readEntity(String.class);        

        Jsonb jsonb = JsonbBuilder.create();

        List<Commit> commitList =
            jsonb.fromJson(content,
                           new ArrayList<Commit>(){}.getClass().getGenericSuperclass());
        
        return commitList;
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

