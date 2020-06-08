package jp.nminoru.amazon_s3_test;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import com.amazonaws.ClientConfiguration;
import com.amazonaws.auth.AWSStaticCredentialsProvider;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.client.builder.AwsClientBuilder.EndpointConfiguration;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3ClientBuilder;
import com.amazonaws.services.s3.model.AmazonS3Exception;
import com.amazonaws.services.s3.model.CopyObjectRequest;
import com.amazonaws.services.s3.model.CopyObjectResult;
import com.amazonaws.services.s3.model.DeleteObjectsRequest;
import com.amazonaws.services.s3.model.DeleteObjectsRequest.KeyVersion;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.ListObjectsV2Request;
import com.amazonaws.services.s3.model.ListObjectsV2Result;
import com.amazonaws.services.s3.model.ObjectMetadata;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.S3ObjectSummary;


public class ClientTest {

    public static String BucketName  = "Bucket";
    public static String Region      = "Region";
    public static String AccessKeyId = "AccessKeyId";
    public static String SecretKey   = "SecretKey";

    public static void main(String[] args) {
        System.out.println("### start ###");

        BucketName  = System.getenv("AMAZON_S3_BUCKET");
        Region      = System.getenv("AMAZON_S3_REGION");
        AccessKeyId = System.getenv("AMAZON_S3_ACCESS_KEY_ID");
        SecretKey   = System.getenv("AMAZON_S3_SECRET_KEY");

        ClientConfiguration clientConf = new ClientConfiguration();

        BasicAWSCredentials credentials =
            new BasicAWSCredentials(AccessKeyId, SecretKey);

        AmazonS3 s3 = AmazonS3ClientBuilder.standard()
            .withRegion(Region)
            .withClientConfiguration(clientConf)
            .withCredentials(new AWSStaticCredentialsProvider(credentials))
            .build();

        if (s3.doesBucketExistV2(BucketName)) {
            System.out.println("### have a bucket ###");

            System.out.println("### listFolder ###");
            listFolder(s3, BucketName);

            //
            System.out.println("### createFolder ###");
            createFolder(s3, BucketName);

            //
            System.out.println("### copyFolder ###");
            copyFolder(s3, BucketName);

            //
            System.out.println("### listFolder ###");
            listFolder(s3, BucketName);

            //
            System.out.println("### etc ###");
            S3Object object = s3.getObject(new GetObjectRequest(BucketName, "def/"));
            System.out.println("key0:  " + object.getKey());

            ObjectMetadata metadata1 = object.getObjectMetadata();
            System.out.println("metadata.type: " + metadata1.getContentType());

            //
            System.out.println("### removeFolder ###");
            deleteFolder(s3, BucketName);
        }

        System.out.println("### end ###");
    }

    static void createFolder(AmazonS3 s3, String bucketName) {
        InputStream input = new ByteArrayInputStream(new byte[0]);
        ObjectMetadata metadata0 = new ObjectMetadata();

        metadata0.setContentLength(0);

        s3.putObject(new PutObjectRequest(bucketName, "def/", input, metadata0));
    }

    static void copyFolder(AmazonS3 s3, String bucketName) {
        ObjectMetadata metadata = s3.getObjectMetadata(bucketName, "def/");

        CopyObjectRequest request = new CopyObjectRequest(bucketName, "def/", bucketName, "efg/");
        request.setNewObjectMetadata(metadata);

        CopyObjectResult result = s3.copyObject(request);
    }

    static void listFolder(AmazonS3 s3, String bucketName) {
        String continuationToken = null;
        ListObjectsV2Result result;

        ListObjectsV2Request request = new ListObjectsV2Request()
            .withBucketName(bucketName)
            // .withPrefix("")
            .withDelimiter("/")
            // .withMaxKeys(5)
            ;

        do {
            result = s3.listObjectsV2(request);

            for (String prefix : result.getCommonPrefixes()) {
                System.out.println("prefix: " + prefix);
            }

            for (S3ObjectSummary summary : result.getObjectSummaries()) {
                System.out.println("key:  " + summary.getKey());
                System.out.println("size: " + summary.getSize());
            }

            continuationToken = result.getNextContinuationToken();
            request.setContinuationToken(continuationToken);
        } while (result.isTruncated());
    }

    static void deleteFolder(AmazonS3 s3, String bucketName) {
        ListObjectsV2Request request = new ListObjectsV2Request()
            .withBucketName(bucketName)
            // .withPrefix("")
            ;

        ListObjectsV2Result result = s3.listObjectsV2(request);

        List<KeyVersion> keys = new ArrayList<KeyVersion>();

        for (S3ObjectSummary summary : result.getObjectSummaries()) {
            System.out.println("key: " + summary.getKey());

            keys.add(new KeyVersion(summary.getKey()));

            if (keys.size() > 0) {
                deleteObjects(s3, BucketName, keys);
                keys = new ArrayList<KeyVersion>();
            }
        }

        if (keys.size() > 0)
            deleteObjects(s3, BucketName, keys);
    }

    static void deleteObjects(AmazonS3 s3, String bucketName, List<KeyVersion> keys) {
        DeleteObjectsRequest request = new DeleteObjectsRequest(bucketName);

        request.setKeys(keys);
        s3.deleteObjects(request);
    }
}
