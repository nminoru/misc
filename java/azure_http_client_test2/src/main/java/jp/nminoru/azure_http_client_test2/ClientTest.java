package jp.nminoru.azure_http_client_test2;

import java.io.InputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;
import java.util.Base64;
import java.util.List;
import java.util.Locale;
import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import org.apache.http.Header;
import org.apache.http.HttpException;
import org.apache.http.HttpHost;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.auth.AuthScheme;
import org.apache.http.auth.AuthSchemeProvider;
import org.apache.http.auth.AuthScope;
import org.apache.http.auth.Credentials;
import org.apache.http.client.CredentialsProvider;
import org.apache.http.client.config.AuthSchemes;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.config.Registry;
import org.apache.http.config.RegistryBuilder;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.NoConnectionReuseStrategy;
import org.apache.http.impl.auth.SPNegoScheme;
import org.apache.http.impl.auth.SPNegoSchemeFactory;
import org.apache.http.impl.client.BasicCredentialsProvider;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.client.HttpClientBuilder;


public class ClientTest {

    public static final String xMsVersion = "2019-12-12";

    public static String AccountName   = "AccountName";
    public static String AccountKey    = "AccountKey";
    public static String ContainerName = "ContainerName";    
    
    static final DateTimeFormatter rfc2616DateFormat = DateTimeFormatter.ofPattern("EEE, dd MMM yyyy HH:mm:ss zzz")
        .withLocale(Locale.ENGLISH)
        .withZone(ZoneId.of("GMT"));

    public static void main(String[] args) throws Exception {
        AccountName   = System.getenv("WASB_ACCOUNT_NAME");
        AccountKey    = System.getenv("WASB_ACCOUNT_KEY");
        ContainerName = System.getenv("WASB_CONTAINER_NAME");

        // System.setProperty("org.apache.commons.logging.Log", "org.apache.commons.logging.impl.SimpleLog");
        // System.setProperty("org.apache.commons.logging.simplelog.showdatetime", "true");
        // System.setProperty("org.apache.commons.logging.simplelog.log.httpclient.wire", "debug");
        // System.setProperty("org.apache.commons.logging.simplelog.log.org.apache.http", "debug");
        // System.setProperty("org.apache.commons.logging.simplelog.log.org.apache.http.headers", "debug");

        String url1      = String.format("https://%s.blob.core.windows.net/%s?restype=container", AccountName, ContainerName);
        String resource1 = String.format("/%s/%s", AccountName, ContainerName);
        
        callApi("GET", url1, Arrays.asList(resource1, "restype:container"));

        // String url2      = String.format("https://%s.blob.core.windows.net/%s?restype=container&comp=list&prefix=folder1&delimiter=/", AccountName, ContainerName);
        String url2      = String.format("https://%s.blob.core.windows.net/%s?restype=container&comp=list&prefix=&delimiter=/", AccountName, ContainerName);        
        String resource2 = String.format("/%s/%s", AccountName, ContainerName);
        
        callApi("GET", url2, Arrays.asList(resource2, "comp:list", "delimiter:/", "prefix:", "restype:container"));

        String url3      = String.format("https://%s.dfs.core.windows.net/%s?resource=filesystem&recursive=false&directory=", AccountName, ContainerName);
        String resource3 = String.format("/%s/%s", AccountName, ContainerName);
        
        callApi("GET", url3, Arrays.asList(resource3, "directory:", "recursive:false", "resource:filesystem"));
    }

    static void callApi(String method, String url, List<String> resources) throws Exception {
        try (CloseableHttpClient httpClient =
             HttpClientBuilder.create().disableRedirectHandling().setConnectionReuseStrategy(new NoConnectionReuseStrategy()).build()) {
            
            HttpGet httpGet  = new HttpGet(url);
            String dateField = rfc2616DateFormat.format(Instant.ofEpochMilli(System.currentTimeMillis()));
            String sharedKey = generateSharedKey(method,
                                                 AccountKey,
                                                 Arrays.asList("x-ms-date:" + dateField, "x-ms-version:" + xMsVersion),
                                                 resources);
            
            httpGet.addHeader("x-ms-date",     dateField);
            httpGet.addHeader("x-ms-version",  xMsVersion);
            httpGet.addHeader("Authorization", "SharedKey " + AccountName + ":" + sharedKey);
            
            try (CloseableHttpResponse httpResponse = httpClient.execute(httpGet)) {
                printResponse(httpResponse);
            }
        }        
    }
    
    static String generateSharedKey(String method, String accessKey, List<String> canonicalizedHeaders, List<String> canonicalizedResources) throws InvalidKeyException, NoSuchAlgorithmException {
        StringBuilder sb = new StringBuilder();

        sb.append(method + "\n"); // VERB
        sb.append("\n");    // Content-Encoding
        sb.append("\n");    // Content-Language
        sb.append("\n");    // Content-Length
        sb.append("\n");    // Content-MD5
        sb.append("\n");    // Content-Type
        sb.append("\n");    // Date
        sb.append("\n");    // If-Modified-Since
        sb.append("\n");    // If-Match        
        sb.append("\n");    // If-None-Match
        sb.append("\n");    // If-Unmodified-Since
        sb.append("\n");    // Range
        for (String header : canonicalizedHeaders)
            sb.append(header + "\n");
        for (String resource : canonicalizedResources)
            sb.append(resource + "\n");

        // 末尾の \n を取り除く
        sb.deleteCharAt(sb.length() - 1);

        String stringToSign = sb.toString();

        Mac sha256HMAC = Mac.getInstance("HmacSHA256");
        sha256HMAC.init(new SecretKeySpec(Base64.getDecoder().decode(accessKey), "HmacSHA256"));

        return Base64.getEncoder().encodeToString(sha256HMAC.doFinal(stringToSign.getBytes(StandardCharsets.UTF_8)));
    }

    static void printResponse(HttpResponse httpResponse) throws IOException {
        System.out.println("status: " + httpResponse.getStatusLine().getStatusCode());

        for (Header header : httpResponse.getAllHeaders()) {
            System.out.println(header.getName() + ": " + header.getValue());
        }

        byte[] bytes = new byte[4096];

        int nbytes;
        InputStream inputStream = httpResponse.getEntity().getContent();

        while ((nbytes = inputStream.read(bytes)) > 0) {
            System.out.write(bytes, 0, nbytes);
        }

        System.out.println();
    }    
}
