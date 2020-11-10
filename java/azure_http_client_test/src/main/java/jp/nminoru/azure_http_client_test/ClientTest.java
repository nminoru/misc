package jp.nminoru.azure_http_client_test;

import java.net.InetSocketAddress;
import com.azure.core.http.HttpMethod;
import com.azure.core.http.HttpRequest;
import com.azure.core.http.HttpResponse;
import com.azure.core.http.HttpClient;
import com.azure.core.http.ProxyOptions;
import com.azure.core.http.netty.NettyAsyncHttpClientBuilder;
import com.azure.core.http.okhttp.OkHttpAsyncHttpClientBuilder;
import reactor.core.publisher.Mono;


public class ClientTest {

    public static final String proxyHost     = "localhost";
    public static final int    proxyPort     = 3128;
    public static final String proxyUser     = "";
    public static final String proxyPassword = "";

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        try {
            clientTest.runNetty();
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }

        try {
            clientTest.runOkHttp();
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }
    }

    public void runNetty() {
        NettyAsyncHttpClientBuilder builder = new NettyAsyncHttpClientBuilder();

        if (true) {
            ProxyOptions proxyOptions = new ProxyOptions(ProxyOptions.Type.HTTP,
                                                         new InetSocketAddress(proxyHost, proxyPort));

            if (!proxyUser.isEmpty() && !proxyPassword.isEmpty())
                proxyOptions.setCredentials(proxyUser, proxyPassword);

            builder.proxy(proxyOptions);
        }

        HttpClient httpClient = builder.build();

        Mono<HttpResponse> mono = httpClient.send(new HttpRequest(HttpMethod.GET, "https://nminoru.blob.core.windows.net"));

        HttpResponse httpResponse = mono.block();

        System.out.println("httpResponse: " + httpResponse);
    }

    public void runOkHttp() {
        OkHttpAsyncHttpClientBuilder builder = new OkHttpAsyncHttpClientBuilder();

        if (true) {
            ProxyOptions proxyOptions = new ProxyOptions(ProxyOptions.Type.HTTP,
                                                         new InetSocketAddress(proxyHost, proxyPort));

            if (!proxyUser.isEmpty() && !proxyPassword.isEmpty())
                proxyOptions.setCredentials(proxyUser, proxyPassword);

            builder.proxy(proxyOptions);
        }

        HttpClient httpClient = builder.build();

        Mono<HttpResponse> mono = httpClient.send(new HttpRequest(HttpMethod.GET, "https://nminoru.blob.core.windows.net"));

        HttpResponse httpResponse = mono.block();

        System.out.println("httpResponse: " + httpResponse);
    }
}
