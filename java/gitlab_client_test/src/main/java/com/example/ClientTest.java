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
import javax.ws.rs.core.GenericType;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.xml.bind.annotation.XmlElement;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import org.glassfish.jersey.client.authentication.HttpAuthenticationFeature;
import org.glassfish.jersey.jackson.JacksonFeature;
import org.glassfish.jersey.jsonb.JsonBindingFeature;
import org.glassfish.jersey.moxy.json.MoxyJsonFeature;
import static org.junit.Assert.*;

public class ClientTest {

    static final String PrivateToken = "qGm5VgrCzA6PpqstUXtb";

    @JsonIgnoreProperties(ignoreUnknown=true)    
    public static class Project {
        public int id;
        public String description;
        public String name;
        public String name_with_namespace;
        public String path_with_namespace;
        public String created_at;
        public String default_branch;
        public String ssh_url_to_repo;
        public String http_url_to_repo;
        public String web_url;
        public String readme_url;
        public String avatar_url;
        public int star_count;
        public int forks_count;
        public String last_activity_at;
        // namespace
        public boolean archived;
        public String visibility;
        // owner
        public boolean resolve_outdated_diff_discussions;
        public boolean container_registry_enabled;
        public boolean issues_enabled;
        public boolean merge_requests_enabled;
        public boolean wiki_enabled;
        public boolean jobs_enabled;
        public boolean snippets_enabled;
        public boolean shared_runners_enabled;
        public boolean lfs_enabled;
        public String import_status;
        public int creator_id;
        public int open_issues_count;
        public boolean public_jobs;
        public List<String> tag_list;
        
        public Project() {
        }

        public Project(String name, int id, String http_url_to_repo, String description, List<String> tag_list) {
            this.name = name;
            this.id = id;
            this.http_url_to_repo = http_url_to_repo;
            this.description = description;
            this.tag_list = tag_list;
        }
    }

    @JsonIgnoreProperties(ignoreUnknown=true)    
    public static class Branch {
        public String name;
        public Commit commit;

        @XmlElement(name = "protected")
        @JsonbProperty(value = "protected")
        public boolean _protected;
        
        public Branch() {
        }

        public Branch(String name, Commit commit) {
            this.name = name;
            this.commit = commit;
        }
    }

    @JsonIgnoreProperties(ignoreUnknown=true)    
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

        Project p;
        
        p = c.createProject("abc");
        p = c.createProject("def");
        p = c.createProject("hij");
        
        List<Project> projects = c.listProjects();

        int lastProjectId = -1;
        
        for (Project project : projects) {

            System.out.println("name: " + project.name);
            System.out.println("id: " + Integer.toString(project.id));
            System.out.println("http_url_to_repo: " + project.http_url_to_repo);
            System.out.println();

            // c.updateProject(project.id, project);
            
            c.listBranches(project.id);
            c.listCommits(project.id);
            
            c.deleteProject(project.id);
            
            lastProjectId = project.id;
        }

        if (lastProjectId > 0)
            c.forkProject(lastProjectId);
    }

    public WebTarget generateTarget() {
        Client client = ClientBuilder.newBuilder().build();
        
        // client.register(MoxyJsonFeature.class);
        // client.register(JsonBindingFeature.class);
        client.register(JacksonFeature.class);

        return client.target("http://127.0.0.1:80/api/v4/");
    }

    public List<Project> listProjects() {
        WebTarget target = generateTarget();

        Response response = target
            .path("/projects")
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .get();

        System.out.println("listProjects: " + response.getStatus());

        // Expected 200 OK
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;

        System.out.println("done");

        return response.readEntity(new GenericType<List<Project>>(){});
    }

    public Project createProject(String projectName) {
        WebTarget target = generateTarget();

        Response response = target        
            .path("/projects")
            .queryParam("name", projectName)
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .post(null);

        System.out.println("createProject: " + response.getStatus());

        // Expected 201 CREATED
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))
            return null;

        System.out.println("done");
        
        return response.readEntity(Project.class);
    }

    public Project updateProject(int projectId, Project request) {
        WebTarget target = generateTarget();

        Response response = target        
            .path(String.format("/projects/%d", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .put(Entity.entity(request, MediaType.APPLICATION_JSON_TYPE));
        
        System.out.println("updateProject: " + response.getStatus());
        
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))        
            return null;        

        System.out.println("done");

        // String content = response.readEntity(String.class);
        // Jsonb jsonb = JsonbBuilder.create();
        // Project project = jsonb.fromJson(content, Project.class);
        // return project;

        return response.readEntity(Project.class);
    }    

    public Project forkProject(int projectId) {
        WebTarget target = generateTarget();

        Response response = target        
            .path(String.format("/projects/%d/fork", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .post(null);

        System.out.println("forkProject: " + response.getStatus());

        System.out.println("\t" + response.getStatus());

        // Expected 201 CREATED
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))        
            return null;

        System.out.println("done");

        return response.readEntity(Project.class);
    }    

    public Project getProjectById(int projectId) {
        WebTarget target = generateTarget();

        Response response = target        
            .path(String.format("/projects/%d", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .get();

        System.out.println("getProject(" + Integer.toString(projectId) + "): " + response.getStatus());

        // Expected 200 OK
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))        
            return null;
        
        System.out.println("done");

        return response.readEntity(Project.class);
    }    

    public List<Branch> listBranches(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d/repository/branches", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .get();

        System.out.println("listBranches: " + response.getStatus());

        // Expected 200 OK
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))        
            return null;
        
        System.out.println("done");
        
        return response.readEntity(new GenericType<List<Branch>>(){});
    }

    public List<Commit> listCommits(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d/repository/commits", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .get();

        System.out.println("listCommits: " + response.getStatus());

        // Expected 200 OK
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))        
            return null;
        
        System.out.println("done");

        return response.readEntity(new GenericType<List<Commit>>(){});
    }    

    public void deleteProject(int projectId) {
        WebTarget target = generateTarget();

        Response response = target
            .path(String.format("/projects/%d", projectId))
            .request(MediaType.APPLICATION_JSON)
            .header("PRIVATE-TOKEN", PrivateToken)
            .delete();

        System.out.println("deleteProject: " + response.getStatus());

        // Expected 202 Accepted
        if (!response.getStatusInfo().getFamily().equals(Response.Status.Family.SUCCESSFUL))        
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

