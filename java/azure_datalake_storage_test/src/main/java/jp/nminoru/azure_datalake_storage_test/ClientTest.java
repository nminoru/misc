package jp.nminoru.azure_datalake_storage_test;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.time.Duration;
import java.util.EnumSet;
import java.util.HashMap;
import com.azure.core.http.HttpClient;
import com.azure.core.http.netty.NettyAsyncHttpClientBuilder;
import com.azure.core.http.okhttp.OkHttpAsyncHttpClientBuilder;
import com.azure.storage.common.StorageSharedKeyCredential;
import com.azure.storage.file.datalake.DataLakeDirectoryClient;
import com.azure.storage.file.datalake.DataLakeFileClient;
import com.azure.storage.file.datalake.DataLakeFileSystemClient;
import com.azure.storage.file.datalake.DataLakeServiceClient;
import com.azure.storage.file.datalake.DataLakeServiceClientBuilder;
import com.azure.storage.file.datalake.models.ListPathsOptions;
import com.azure.storage.file.datalake.models.PathItem;
import com.azure.storage.file.datalake.models.PathProperties;
import org.apache.commons.io.IOUtils;

import jp.nminoru.azure.core.http.apache.httpclient.ApacheHttpClientBuilder;


public class ClientTest {

    public static String AccountName   = "AccountName";
    public static String AccountKey    = "AccountKey";
    public static String ContainerName = "ContainerName";

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        clientTest.run();
    }

    public HttpClient getHttpClient() {
        // NettyAsyncHttpClientBuilder  builder = new NettyAsyncHttpClientBuilder();
        // OkHttpAsyncHttpClientBuilder builder = new OkHttpAsyncHttpClientBuilder();
        ApacheHttpClientBuilder builder = new ApacheHttpClientBuilder();

        return builder.build();
    }

    public void run() {
        AccountName   = System.getenv("ABFS_ACCOUNT_NAME");
        AccountKey    = System.getenv("ABFS_ACCOUNT_KEY");
        ContainerName = System.getenv("ABFS_CONTAINER_NAME");

	System.out.println("ABFS_ACCOUNT_NAME   = " + AccountName);
	System.out.println("ABFS_ACCOUNT_KEY    = " + AccountKey);
	System.out.println("ABFS_CONTAINER_NAME = " + ContainerName);

        String abfs_uri = String.format("https://%s.dfs.core.windows.net/", AccountName);

        StorageSharedKeyCredential storageSharedKeyCredential =
            new StorageSharedKeyCredential(AccountName, AccountKey);

        DataLakeServiceClient dataLakeServiceClient =
            new DataLakeServiceClientBuilder()
                .endpoint(abfs_uri)
                .httpClient(getHttpClient())
                .credential(storageSharedKeyCredential)
                .buildClient();

        DataLakeFileSystemClient dataLakeFileSystemClient =
            dataLakeServiceClient.getFileSystemClient(ContainerName);

        DataLakeDirectoryClient directoryClient = dataLakeFileSystemClient.getDirectoryClient("");
        // PathProperties properties = directoryClient.getProperties();

        ListPathsOptions listPathsOptions = new ListPathsOptions();
        listPathsOptions.setPath("");
        listPathsOptions.setRecursive(false);

        for (PathItem pathItem : dataLakeFileSystemClient.listPaths(listPathsOptions, Duration.ofMinutes(5))) {
            System.out.println("path: " + pathItem.getName());
        }

        System.out.println("#1 done");

        String word123 = "123";

        DataLakeFileClient newFile = dataLakeFileSystemClient.createFile("123", true);

        newFile.append(new ByteArrayInputStream(word123.getBytes(StandardCharsets.UTF_8)), 0, 3);
        newFile.flush(3);

        System.out.println("#2 done");

        dataLakeFileSystemClient.deleteFile("123");

        System.out.println("#3 done");

        dataLakeFileSystemClient.createDirectory("123456");

        System.out.println("#4 done");

        dataLakeFileSystemClient.deleteDirectory("123456");

        System.out.println("#5 done");
    }
}
