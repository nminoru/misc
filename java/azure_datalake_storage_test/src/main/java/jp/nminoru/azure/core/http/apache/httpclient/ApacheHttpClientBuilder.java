package jp.nminoru.azure.core.http.apache.httpclient;

import java.io.IOException;
import java.io.Reader;
import java.net.Authenticator;
import java.net.PasswordAuthentication;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.Duration;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;
import java.util.Properties;
import java.util.Set;
import java.util.concurrent.Executor;
import java.util.stream.Collectors;

import com.azure.core.http.HttpClient;
import com.azure.core.http.ProxyOptions;
import com.azure.core.util.Configuration;
import com.azure.core.util.logging.ClientLogger;

import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.client.HttpClientBuilder;


public class ApacheHttpClientBuilder {

    private final static ClientLogger LOGGER = new ClientLogger(ApacheHttpClientBuilder.class);

    static final Set<String> DEFAULT_RESTRICTED_HEADERS;

    static {
        Set<String> hashSet = new HashSet<String>() {
            {
                add("connection");
                add("content-length");
                add("content-type");
                add("expect");
                add("host");
                add("upgrade");
            }
        };

        DEFAULT_RESTRICTED_HEADERS = Collections.unmodifiableSet(hashSet);
    }

    private ProxyOptions proxyOptions;

    public ApacheHttpClientBuilder() {
    }

    public ApacheHttpClientBuilder proxy(ProxyOptions proxyOptions) {
        // proxyOptions can be null
        this.proxyOptions = proxyOptions;

        return this;
    }

    public HttpClient build() {
        HttpClientBuilder httpClientBuilder = HttpClientBuilder.create();

        // ProxyOptions buildProxyOptions = (proxyOptions == null && buildConfiguration != Configuration.NONE)
        // ? ProxyOptions.fromConfiguration(buildConfiguration)
        // : proxyOptions;

        return new ApacheHttpClient(httpClientBuilder.build(), DEFAULT_RESTRICTED_HEADERS);
    }

    private static class ProxyAuthenticator extends Authenticator {
        private final String userName;
        private final String password;

        ProxyAuthenticator(String userName, String password) {
            this.userName = userName;
            this.password = password;
        }

        @Override
        protected PasswordAuthentication getPasswordAuthentication() {
            return new PasswordAuthentication(this.userName, password.toCharArray());
        }
    }
}
