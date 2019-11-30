package jp.nminoru.azure_blob_storage_test;

import java.io.InputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.EnumSet;
import java.util.HashMap;
import com.microsoft.azure.storage.StorageCredentials;
import com.microsoft.azure.storage.StorageCredentialsAccountAndKey;
import com.microsoft.azure.storage.StorageException;
import com.microsoft.azure.storage.StorageUri;
import com.microsoft.azure.storage.blob.BlobContainerPermissions;
import com.microsoft.azure.storage.blob.BlobContainerPublicAccessType;
import com.microsoft.azure.storage.blob.BlobListingDetails;
import com.microsoft.azure.storage.blob.BlobProperties;
import com.microsoft.azure.storage.blob.BlobRequestOptions;
import com.microsoft.azure.storage.blob.BlobType;
import com.microsoft.azure.storage.blob.BlockEntry;
import com.microsoft.azure.storage.blob.CloudAppendBlob;
import com.microsoft.azure.storage.blob.CloudBlob;
import com.microsoft.azure.storage.blob.CloudBlobClient;
import com.microsoft.azure.storage.blob.CloudBlobContainer;
import com.microsoft.azure.storage.blob.CloudBlobDirectory;
import com.microsoft.azure.storage.blob.CloudBlockBlob;
import com.microsoft.azure.storage.blob.CloudPageBlob;
import com.microsoft.azure.storage.blob.CopyState;
import com.microsoft.azure.storage.blob.CopyStatus;
import com.microsoft.azure.storage.blob.DeleteSnapshotsOption;
import com.microsoft.azure.storage.blob.ListBlobItem;
import com.microsoft.azure.storage.blob.PageRange;
import org.apache.commons.io.IOUtils;


public class ClientTest {

    static final EnumSet<BlobListingDetails> details = EnumSet.of(BlobListingDetails.METADATA);
    static final BlobRequestOptions options = new BlobRequestOptions();

    public static String AccountName   = "AccountName";
    public static String AccountKey    = "AccountKey";
    public static String ContainerName = "ContainerName";

    public static void main(String[] args) throws InterruptedException, IOException, URISyntaxException, StorageException {
        ClientTest clientTest = new ClientTest();

        clientTest.run();
    }

    public void run() throws InterruptedException, IOException, URISyntaxException, StorageException {
        AccountName   = System.getenv("WASB_ACCOUNT_NAME");
        AccountKey    = System.getenv("WASB_ACCOUNT_KEY");
        ContainerName = System.getenv("WASB_CONTAINER_NAME");

        StorageCredentials  credentials    = new StorageCredentialsAccountAndKey(AccountName, AccountKey);
        CloudBlobClient     blobClient     = new CloudBlobClient(new StorageUri(URI.create("https://nminoru.blob.core.windows.net/")), credentials);
        CloudBlobContainer  container      = blobClient.getContainerReference(ContainerName);

        // container.createIfNotExists();

        // CloudBlockBlob blob = container.getBlockBlobReference(fileName);
        // File source = new File(filePath);
        // blob.upload(new FileInputStream(source), source.length());

        // blob.openOutputStream()
        // blob.download(OutputStream outStream)

        // blob.exists()
        // blob.delete()
        // blob.deleteIfExists()
        // BlobProperties blob.getProperties()

        if (!container.exists()) {
            System.err.println("the specified container is not exists");
            return;
        }

        // ディレクトリの削除
        deleteFolder(container, "foo/");

        System.out.println("=== STEP 1 ===");
        listFolder(container, "", false, false);

        HashMap<String, String> metadata = new HashMap<String, String>() {{put("hdi_isfolder", "true");}};

        // ディレクトの作成
        CloudBlockBlob blob1;
        blob1 = container.getBlockBlobReference("foo/bar/");
        blob1.setMetadata(metadata); // 必須ではない
        blob1.uploadFromByteArray(new byte[0], 0, 0);

        // ファイルの作成
        CloudBlockBlob blob2;
        blob2 = container.getBlockBlobReference("foo/test.txt");
        try (InputStream inputStream = ClientTest.class.getResourceAsStream("test.txt")) {
            blob2.upload(inputStream, -1);
        }

        // ファイルのダウンロード
        try (InputStream inputStream1 = blob2.openInputStream();
             InputStream inputStream2 = ClientTest.class.getResourceAsStream("test.txt")) {
            boolean result = IOUtils.contentEquals(inputStream1, inputStream2);
            System.out.println("result = " + result);
        }

        // ファイルの作成
        CloudBlockBlob blob3;
        blob3 = container.getBlockBlobReference("foo/bar/baz/test.txt");
        try (InputStream inputStream = ClientTest.class.getResourceAsStream("test.txt")) {
            blob3.upload(inputStream, -1);
        }

        System.out.println("=== STEP 2 ===");
        listFolder(container, "", false, false);

        System.out.println("=== STEP 3 ===");
        getCloudBlob(container.getBlockBlobReference("foo/bar/baz/test.txt"));

        // com.microsoft.azure.storage.StorageException

        System.out.println("=== STEP 4.1 ===");
        try {
            getCloudBlob(container.getBlobReferenceFromServer("foo/bar/baz/test.txt"));
        } catch (StorageException e) {
            System.err.println("status = " + e.getHttpStatusCode());
            throw e;
        }

        System.out.println("=== STEP 4.2 ===");
        try {
            getCloudBlob(container.getBlobReferenceFromServer("foo/bar/"));
        } catch (StorageException e) {
            System.err.println("status = " + e.getHttpStatusCode());
            throw e;
        }

        System.out.println("=== STEP 4.3 ===");
        try {
            getCloudBlob(container.getBlobReferenceFromServer("foo/bar/baz/"));
        } catch (StorageException e) {
            System.err.println("status = " + e.getHttpStatusCode());
            // throw e;
        }

        // ファイルのコピー
        System.out.println("=== STEP 5 ===");
        copyCloudBlob(blob3, container.getBlockBlobReference("foo/test2.txt"));

        System.out.println("=== STEP 6 ===");
        CloudBlockBlob blob4;
        blob4 = container.getBlockBlobReference("foo/bar/");
        copyCloudBlob(blob3, container.getBlockBlobReference("bar/"));

        System.out.println("=== STEP 7 ===");
        copyFolders(container, "foo/bar/", "bar4/");

        System.out.println("=== STEP 8 ===");
        listFolder(container, "新しいフォルダー/", false, false);

        System.out.println("=== STEP 9 ===");
        listFolder(container, "", true, false);

        System.out.println("=== FINAL ===");
        deleteFolder(container, "bar/");
        deleteFolder(container, "foo/");
        deleteFolder(container, "foo3/");
        deleteFolder(container, "foo4/");
    }

    private void listFolder(CloudBlobContainer container, String name, boolean useFlatBlobListing, boolean recursive) {
        // ディレクトリのリスティング
        for (ListBlobItem blobItem : container.listBlobs(name, useFlatBlobListing, details, options, null)) {
            System.out.println(blobItem.getUri());

            if (blobItem instanceof CloudBlob) {
                CloudBlob cloudBlob = (CloudBlob)blobItem;
                BlobProperties properties = cloudBlob.getProperties();

                // getParent() CloudBlobDirectory
                // StorageUri getSnapshotQualifiedStorageUri()

                System.out.println("BLOB: " + cloudBlob.getName());
                System.out.println("\t" + properties.getLength());
                System.out.println("\t" + properties.getCreatedTime());
                System.out.println("\t" + properties.getLastModified());
            } else if (blobItem instanceof CloudBlobDirectory) {
                // BlobProperties properties = blobItem.getProperties()
                CloudBlobDirectory cloudBlobDirectory = (CloudBlobDirectory)blobItem;

                System.out.println("VDIR: " + cloudBlobDirectory.getPrefix());

                if (recursive)
                    listFolder(container, cloudBlobDirectory.getPrefix(), useFlatBlobListing, recursive);
            }
        }
    }

    private void copyFolders(CloudBlobContainer container, String src, String destBase) throws InterruptedException, URISyntaxException, StorageException {
        _copyFolders(container, src, src.length(), destBase);
    }

    private void _copyFolders(CloudBlobContainer container, String src, int srcLength, String destBase) throws InterruptedException, URISyntaxException, StorageException {
        for (ListBlobItem blobItem : container.listBlobs(src, true)) {
            if (blobItem instanceof CloudBlob) {
                CloudBlob child = (CloudBlob)blobItem;

                String name = child.getName();
                String destName = destBase + name.substring(0, name.length() - srcLength);

                copyCloudBlob(child, getCloudBlob(container, child.getProperties().getBlobType(), destName));
            } else if (blobItem instanceof CloudBlobDirectory) {
                CloudBlobDirectory child = (CloudBlobDirectory)blobItem;

                String name = child.getPrefix();
                _copyFolders(container, name, srcLength, destBase);
            }
        }
    }

    private void deleteFolder(CloudBlobContainer container, String folder) throws URISyntaxException, StorageException {
        for (ListBlobItem blobItem : container.listBlobs(folder, true)) {
            if (blobItem instanceof CloudBlob) {
                CloudBlob cloudBlob = (CloudBlob)blobItem;
                cloudBlob.delete();
            } else if (blobItem instanceof CloudBlobDirectory) {
                CloudBlobDirectory cloudBlobDirectory = (CloudBlobDirectory)blobItem;
                deleteFolder(container, cloudBlobDirectory.getPrefix());
            }
        }

        CloudBlobDirectory directory;
        directory = container.getDirectoryReference(folder);
        for (ListBlobItem blobItem : directory.listBlobs()) {
            if (blobItem instanceof CloudBlob) {
                CloudBlob cloudBlob = (CloudBlob)blobItem;

                cloudBlob.deleteIfExists();
            }
        }
    }

    private void getCloudBlob(CloudBlob cblob) throws StorageException {
        System.out.println(cblob.exists());

        System.out.println("name: " + cblob.getName());

        BlobProperties properties = cblob.getProperties();
        System.out.println("properties.blobType: "     + properties.getBlobType());
        System.out.println("properties.createdTime: "  + properties.getCreatedTime());
        System.out.println("properties.lastModified: " + properties.getLastModified());
        System.out.println("properties.length: "       + properties.getLength());

        HashMap<String, String> metadata2 = cblob.getMetadata();
        for (String key : metadata2.keySet()) {
            System.out.println("key: " + key + " = value: " + metadata2.get(key));
        }
    }

    private void copyCloudBlob(CloudBlob srcBlob, CloudBlob destBlob) throws InterruptedException, URISyntaxException, StorageException {
        String copyId = destBlob.startCopy(srcBlob.getUri());
        // abortCopy(copyId);
        CopyState copyState = destBlob.getCopyState();

        while (copyState.getStatus().equals(CopyStatus.PENDING)) {
            Thread.sleep(1000);
        }
    }

    private CloudBlob getCloudBlob(CloudBlobContainer container, BlobType blobType, String path) throws URISyntaxException, StorageException {
        switch (blobType) {
        case APPEND_BLOB:
            return container.getAppendBlobReference(path);
        case BLOCK_BLOB:
            return container.getBlockBlobReference(path);
        case PAGE_BLOB:
            return container.getPageBlobReference(path);
        default:
            throw new EnumConstantNotPresentException(blobType.getClass(), blobType.toString());
        }
    }
}
