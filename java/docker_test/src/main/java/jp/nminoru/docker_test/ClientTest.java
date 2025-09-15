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
import com.spotify.docker.client.messages.ContainerStats;
import com.spotify.docker.client.messages.ExecCreation;
import com.spotify.docker.client.messages.HostConfig;
import com.spotify.docker.client.messages.NetworkSettings;
import com.spotify.docker.client.messages.NetworkStats;
import com.spotify.docker.client.messages.PortBinding;
import com.spotify.docker.client.messages.Image;


public class ClientTest {

    // public static final String TARGET_IMAGE_NAME = "jupyter/scipy-notebook:latest";
    public static final String TARGET_IMAGE_NAME = "redis";

    public static void main(String[] args) throws DockerCertificateException, DockerException, InterruptedException {
	System.out.println("test start");
	
        ClientTest clientTest = new ClientTest();

        // clientTest.test1(TARGET_IMAGE_NAME);
        clientTest.test2();
    }

    final DockerClient client;

    public ClientTest() throws DockerCertificateException, DockerException {
        // client = DefaultDockerClient.fromEnv().build();
        client = DefaultDockerClient.builder().uri("http://127.0.0.1:2376/").build();
    }

    public void test1(String imageName) throws DockerException, InterruptedException {

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

        final ContainerCreation creation = client.createContainer(containerConfig, "hogehoge");
        final String containerId = creation.id();
        final LogStream logs = client.logs(containerId,
                                           DockerClient.LogsParam.stdout(),
                                           DockerClient.LogsParam.stderr(),
                                           DockerClient.LogsParam.follow());

        System.out.println("Container ID: " + containerId);

        inspect(containerId);

        System.out.println("Phase: start container");

        // Inspect container

        // Start container
        client.startContainer(containerId);

        inspect(containerId);

        System.out.println(logs.readFully());

        try {
            System.in.read();
        } catch (IOException e) {
        }

        System.out.println(logs.readFully());

        inspect(containerId);
        
        // Exec command inside running container with attached STDOUT and STDERR
        // final String[] command = {"sh", "-c", "ls"};
        // final ExecCreation execCreation = docker.execCreate(id, command, DockerClient.ExecCreateParam.attachStdout(),
        // DockerClient.ExecCreateParam.attachStderr());
        // final LogStream output = docker.execStart(execCreation.id());
        // 

        System.out.println("Phase: stop container");
        
        // Kill container
        client.killContainer(containerId);

        System.out.println("Phase: remove container");

        // Remove container
        client.removeContainer(containerId);

        // Close the docker client
        client.close();
    }

    void inspect(String containerId) throws DockerException, InterruptedException {
        final ContainerInfo info = client.inspectContainer(containerId);

        System.out.println("[Name]");
        System.out.println("\t" + info.name());

        System.out.println("[Networks]");
        NetworkSettings networkSettings = info.networkSettings();

        for (Map.Entry<String, List<PortBinding>> entry : networkSettings.ports().entrySet()) {
            System.out.println("\t" + entry.getKey());
            for (PortBinding portBinding : entry.getValue()) {
                System.out.println("\t\t" + portBinding.hostPort());
            }
        }
        System.out.println();

        final ContainerStats stats = client.stats(containerId);

        long rxBytes = 0;        
        long txBytes = 0;
        
        if (stats.network() != null) {
            NetworkStats networkStats = stats.network();
            
            rxBytes += networkStats.rxBytes();
            txBytes += networkStats.txBytes();
        }

        if (stats.networks() != null) {
            for (String key : stats.networks().keySet()) {
                NetworkStats networkStats = stats.networks().get(key);
                
                rxBytes += networkStats.rxBytes();
                txBytes += networkStats.txBytes();
            }
        }

        System.out.println("\trxBytes: " + rxBytes);
        System.out.println("\ttxBytes: " + txBytes);        
    }

    /**
     * Docker 20.10 で /images/json のパラメータが使えなくなったのの確認
     */
    public void test2() throws DockerException, InterruptedException {
        List<Image> imageList;

        // byName を指定してイメージをリストアップする
        imageList = client.listImages(DockerClient.ListImagesParam.byName(TARGET_IMAGE_NAME));
        for (Image image : imageList) {
            System.out.println("image: " + image.id());
            for (String digest : image.repoDigests()) {
                System.out.println("\tdigest: " + digest);
            }            
            for (String tag : image.repoTags()) {
                System.out.println("\ttag: " + tag);
            }
            if (image.labels() != null) {
                for (Map.Entry<String, String> entry : image.labels().entrySet()) {
                    System.out.println("\tlabel: " + entry.getKey() + " = " + entry.getValue());
                }
            }
        }
            
        // List<Image> images = new ArrayList<>();
        // List<ListImagesParam> params = new ArrayList<>();
        // for (Entry<String, String> entry : labels.entrySet()) {
        // params.add(DockerClient.ListImagesParam.withLabel(entry.getKey(), entry.getValue()));
        // }
        // images = client.listImages(params.toArray(new ListImagesParam[0]));        
    }
}
