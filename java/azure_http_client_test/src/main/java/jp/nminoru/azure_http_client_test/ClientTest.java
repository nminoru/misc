package jp.nminoru.azure_http_client_test;

import java.net.InetSocketAddress;
import reactor.core.publisher.Mono;
import com.azure.core.http.HttpMethod;
import com.azure.core.http.HttpRequest;
import com.azure.core.http.HttpResponse;
import com.azure.core.http.HttpClient;
import com.azure.core.http.ProxyOptions;
import com.azure.core.http.netty.NettyAsyncHttpClientBuilder;


public class ClientTest {

    public static void main(String[] args) {
        ClientTest clientTest = new ClientTest();

        clientTest.run();
    }

    public void run() {
        NettyAsyncHttpClientBuilder builder = new NettyAsyncHttpClientBuilder();

        if (true) {
            ProxyOptions proxyOptions = new ProxyOptions(ProxyOptions.Type.HTTP,
                                                         new InetSocketAddress("localhost", 3128));

            // String user     = "user";
            // String password = "password";
            // 
            // if (!user.isEmpty() && !password.isEmpty())
            //     proxyOptions.setCredentials(user, password);

            builder.proxy(proxyOptions);
        }

        HttpClient httpClient = builder.build();;
        
        Mono<HttpResponse> mono = httpClient.send(new HttpRequest(HttpMethod.GET, "https://nminoru.blob.core.windows.net"));

        HttpResponse httpResponse = mono.block();

        System.out.println("httpResponse: " + httpResponse);
    }
}
