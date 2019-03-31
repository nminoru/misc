package jp.nminoru.docker_test;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import com.spotify.docker.client.DefaultDockerClient;
import com.spotify.docker.client.DockerClient;
import com.spotify.docker.client.LogStream;
import com.spotify.docker.client.exceptions.DockerCertificateException;
import com.spotify.docker.client.exceptions.DockerException;
import com.spotify.docker.client.messages.ContainerConfig;
import com.spotify.docker.client.messages.ContainerCreation;
import com.spotify.docker.client.messages.ContainerInfo;
import com.spotify.docker.client.messages.ExecCreation;
import com.spotify.docker.client.messages.HostConfig;
import com.spotify.docker.client.messages.NetworkSettings;
import com.spotify.docker.client.messages.PortBinding;


public class ClientTest {

    public static final String TARGET_IMAGE_NAME = "jupyter/datascience-notebook";
    // public static final String TARGET_IMAGE_NAME = "busybox";

    public static void main(String[] args) throws DockerCertificateException, DockerException, InterruptedException {
        ClientTest clientTest = new ClientTest();

        clientTest.test(TARGET_IMAGE_NAME);
    }

    final DockerClient docker;

    public ClientTest() throws DockerCertificateException, DockerException {
        docker = DefaultDockerClient.fromEnv().build();
    }

    public void test(String imageName) throws DockerException, InterruptedException {

        // Pull an image
        
        // System.out.println("Phase: pull image");         
        // docker.pull(imageName);
        
        System.out.println("Phase: create container"); 

        // Bind container ports to host ports
        final String[] ports = {"8888"};
        final Map<String, List<PortBinding>> portBindings = new HashMap<>();
        for (String port : ports) {
            List<PortBinding> hostPorts = new ArrayList<>();

            PortBinding portBinding;

            // PortBinding.of("0.0.0.0", port);
            portBinding = PortBinding.randomPort("0.0.0.0");
            
            hostPorts.add(portBinding);
            portBindings.put(port, hostPorts);
        }
        
        final HostConfig hostConfig = HostConfig.builder().portBindings(portBindings).build();

        // Create container with exposed ports
        final ContainerConfig containerConfig = ContainerConfig.builder()
            .hostConfig(hostConfig)
            .image(imageName)
            .exposedPorts(ports)
            .labels(new HashMap<String, String>(){
                    {
                        put("jp.nminoru.docker_test.test", "test");
                    }
                })
            .build();

        final ContainerCreation creation = docker.createContainer(containerConfig);
        final String containerId = creation.id();
        final LogStream logs = docker.logs(containerId,
                                           DockerClient.LogsParam.stdout(),
                                           DockerClient.LogsParam.stderr(),
                                           DockerClient.LogsParam.follow());

        System.out.println("Container ID: " + containerId);

        inspect(containerId);

        System.out.println("Phase: start container");

        // Inspect container

        // Start container
        docker.startContainer(containerId);

        inspect(containerId);        

        System.out.println(logs.readFully());

        try {
            System.in.read();
        } catch (IOException e) {
        }

        System.out.println(logs.readFully());        

        // Exec command inside running container with attached STDOUT and STDERR
        // final String[] command = {"sh", "-c", "ls"};
        // final ExecCreation execCreation = docker.execCreate(id, command, DockerClient.ExecCreateParam.attachStdout(),
        // DockerClient.ExecCreateParam.attachStderr());
        // final LogStream output = docker.execStart(execCreation.id());
        // 

        System.out.println("Phase: stop container");
        
        // Kill container
        docker.killContainer(containerId);

        System.out.println("Phase: remove container");

        // Remove container
        docker.removeContainer(containerId);

        // Close the docker client
        docker.close();
    }

    void inspect(String containerId) throws DockerException, InterruptedException {
        final ContainerInfo info = docker.inspectContainer(containerId);

        NetworkSettings networkSettings = info.networkSettings();

        for (Map.Entry<String, List<PortBinding>> entry : networkSettings.ports().entrySet()) {
            System.out.println("\t" + entry.getKey());
            for (PortBinding portBinding : entry.getValue()) {
                System.out.println("\t\t" + portBinding.hostPort());
            }
        }
        System.out.println();        
    }
}
