package jp.nminoru.azure_datalake_storage_test2;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.time.Duration;
import java.util.EnumSet;
import java.util.HashMap;
import com.azure.core.http.HttpClient;
import com.azure.core.http.ProxyOptions;
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


public class ClientTest {

    String accountName   = "AccountName";
    String accountKey    = "AccountKey";
    String containerName = "ContainerName";
    String proxyHost;
    String proxyPort;

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        clientTest.accountName   = System.getenv("ABFS_ACCOUNT_NAME");
        clientTest.accountKey    = System.getenv("ABFS_ACCOUNT_KEY");
        clientTest.containerName = System.getenv("ABFS_CONTAINER_NAME");

        clientTest.proxyHost     = System.getenv("PROXY_HOST");
        clientTest.proxyPort     = System.getenv("PROXY_PORT");

        clientTest.run();
    }

    public HttpClient getHttpClient() {
        OkHttpAsyncHttpClientBuilder builder = new OkHttpAsyncHttpClientBuilder();

        if (proxyHost != null && !proxyHost.isEmpty()) {
            System.out.println("proxy: " + proxyHost + ":" + proxyPort);
            ProxyOptions proxyOptions =
                new ProxyOptions(ProxyOptions.Type.HTTP,
                                 new InetSocketAddress(proxyHost, Integer.parseInt(proxyPort)));
             builder.proxy(proxyOptions);
        }

        return builder.build();
    }

    public void run() {
        String abfs_uri = String.format("https://%s.dfs.core.windows.net/", accountName);

        System.out.println("abfs_uri = " + abfs_uri);

        StorageSharedKeyCredential storageSharedKeyCredential =
            new StorageSharedKeyCredential(accountName, accountKey);

        DataLakeServiceClient dataLakeServiceClient =
            new DataLakeServiceClientBuilder()
                .endpoint(abfs_uri)
                .httpClient(getHttpClient())
                .credential(storageSharedKeyCredential)
                .buildClient();

        DataLakeFileSystemClient dataLakeFileSystemClient =
            dataLakeServiceClient.getFileSystemClient(containerName);

        DataLakeDirectoryClient directoryClient = dataLakeFileSystemClient.getDirectoryClient("");

        ListPathsOptions listPathsOptions = new ListPathsOptions();
        listPathsOptions.setPath("");
        listPathsOptions.setRecursive(false);

        for (PathItem pathItem : dataLakeFileSystemClient.listPaths(listPathsOptions, Duration.ofMinutes(5))) {
            System.out.println("path: " + pathItem.getName());
        }

        System.out.println("done");
    }
}
