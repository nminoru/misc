package jp.nminoru.docker_test2;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Collections;
import com.github.dockerjava.api.DockerClient;
import com.github.dockerjava.api.command.CreateContainerCmd;
import com.github.dockerjava.api.command.InspectImageResponse;
import com.github.dockerjava.api.command.InspectContainerResponse;
import com.github.dockerjava.api.exception.ConflictException;
import com.github.dockerjava.api.exception.NotFoundException;
import com.github.dockerjava.api.model.Bind;
import com.github.dockerjava.api.model.Container;
import com.github.dockerjava.api.model.DeviceRequest;
import com.github.dockerjava.api.model.ExposedPort;
import com.github.dockerjava.api.model.Image;
import com.github.dockerjava.core.DefaultDockerClientConfig;
import com.github.dockerjava.core.DockerClientConfig;
import com.github.dockerjava.core.DockerClientImpl;
import com.github.dockerjava.httpclient5.ApacheDockerHttpClient;
import com.github.dockerjava.transport.DockerHttpClient;



public class ClientTest {

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        // clientTest.test1(TARGET_IMAGE_NAME);
        // clientTest.test2("postgres");
        // clientTest.test3("katsuo.container.type", "ProjectContainer", false);
        clientTest.test4("523528384b06", "df72217aa7ae");
    }

    final DockerClient anotherClient;

    public ClientTest() {
        DockerClientConfig custom = DefaultDockerClientConfig.createDefaultConfigBuilder()
                    .withDockerHost("tcp://127.0.0.1:2376/").withDockerTlsVerify(false).build();
        DockerHttpClient httpClient = new ApacheDockerHttpClient.Builder().dockerHost(custom.getDockerHost())
                    .build();
        
        anotherClient = DockerClientImpl.getInstance(custom, httpClient);
    }

    /**
     * Docker 20.10 で /images/json のパラメータが使えなくなったのの確認
     */
    public void test2(String imageName) {
        System.out.println("test2: list image specified name");
                           
        List<Image> imageList = anotherClient.listImagesCmd().withImageNameFilter(imageName).withShowAll(true).exec();
        
        for (Image image : imageList) {
            System.out.println("image: " + image.getId());
            for (String digest : image.getRepoDigests()) {
                System.out.println("\tdigest: " + digest);
            }            
            for (String tag : image.getRepoTags()) {
                System.out.println("\ttag: " + tag);
            }
            if (image.getLabels() != null) {
                for (Map.Entry<String, String> entry : image.getLabels().entrySet()) {
                    System.out.println("\tlabel: " + entry.getKey() + " = " + entry.getValue());
                }
            }
        }
    }

    public void test3(String label, String value, boolean withShowAll) {
        System.out.println("test3: list containers specified label");
        
        List<Container> containerList = anotherClient.listContainersCmd().withLabelFilter(Collections.singletonMap(label, value)).withShowAll(withShowAll).exec();

        for (Container container : containerList) {
            String dockerProjectName = container.getLabels().get("katsuo.container.project");
            System.out.println("scan container: " + container.getId() + ", " + dockerProjectName);
        }
    }

    public void test4(String imageId, String containerId) {
        System.out.println("test4: ");

        try {
            InspectImageResponse imageRes = anotherClient.inspectImageCmd(imageId).exec();
            System.out.println("image: " + (imageRes != null));
        } catch (NotFoundException e) {
            System.out.println("test4: image not found");            
        }

        try {        
            InspectContainerResponse containerRes = anotherClient.inspectContainerCmd(containerId).exec();
            System.out.println("container: " + (containerRes != null));
        } catch (NotFoundException e) {
            System.out.println("test4: container not found");
        }
    }        
}
