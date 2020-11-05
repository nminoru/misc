package jp.nminoru.azure_datalake_storage_test;

import java.io.InputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.EnumSet;
import java.time.Duration;
import java.util.HashMap;
import com.azure.storage.common.StorageSharedKeyCredential;
import com.azure.storage.file.datalake.DataLakeDirectoryClient;
import com.azure.storage.file.datalake.DataLakeFileSystemClient;
import com.azure.storage.file.datalake.DataLakeServiceClient;
import com.azure.storage.file.datalake.DataLakeServiceClientBuilder;
import com.azure.storage.file.datalake.models.ListPathsOptions;
import com.azure.storage.file.datalake.models.PathItem;
import org.apache.commons.io.IOUtils;


public class ClientTest {

    public static String AccountName   = "AccountName";
    public static String AccountKey    = "AccountKey";
    public static String ContainerName = "ContainerName";

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        clientTest.run();
    }

    public void run() {
        AccountName   = System.getenv("ABFS_ACCOUNT_NAME");
        AccountKey    = System.getenv("ABFS_ACCOUNT_KEY");
        ContainerName = System.getenv("ABFS_CONTAINER_NAME");

        String abfs_uri = String.format("https://%s.dfs.core.windows.net/", AccountName);

        StorageSharedKeyCredential storageSharedKeyCredential =
            new StorageSharedKeyCredential(AccountName, AccountKey);

        DataLakeServiceClient dataLakeServiceClient =
            new DataLakeServiceClientBuilder()
                .endpoint(abfs_uri)
                .credential(storageSharedKeyCredential)
                .buildClient();

        DataLakeFileSystemClient dataLakeFileSystemClient =
            dataLakeServiceClient.getFileSystemClient(ContainerName);

        ListPathsOptions listPathsOptions = new ListPathsOptions();
        listPathsOptions.setPath("");
        listPathsOptions.setRecursive(false);

        for (PathItem pathItem : dataLakeFileSystemClient.listPaths(listPathsOptions, Duration.ofMinutes(5))) {
            System.out.println("path: " + pathItem.getName());
        }

        System.out.println("done");
    }
}
