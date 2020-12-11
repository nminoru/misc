package jp.nminoru.azure.core.http.apache.httpclient;

import java.io.InputStream;
import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.Set;

import com.azure.core.http.HttpClient;
import com.azure.core.http.HttpHeader;
import com.azure.core.http.HttpHeaders;
import com.azure.core.http.HttpRequest;
import com.azure.core.http.HttpResponse;
import com.azure.core.util.Context;
import com.azure.core.util.CoreUtils;
import com.azure.core.util.FluxUtil;
import com.azure.core.util.logging.ClientLogger;
import reactor.core.Exceptions;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;
import org.apache.http.Header;
import org.apache.http.HttpHost;
import org.apache.http.ProtocolVersion;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.message.BasicHttpEntityEnclosingRequest;



public class ApacheHttpClient implements HttpClient {
    private final static ClientLogger LOGGER = new ClientLogger(ApacheHttpClient.class);

    public static final ProtocolVersion HTTP_1_1 = new ProtocolVersion("HTTP", 1, 1);

    private final CloseableHttpClient httpClient;

    private final Set<String> restrictedHeaders;

    ApacheHttpClient(CloseableHttpClient httpClient, Set<String> restrictedHeaders) {
        this.httpClient        = httpClient;
        this.restrictedHeaders = restrictedHeaders;
    }

    @Override
    public Mono<HttpResponse> send(HttpRequest request) {
        return send(request, Context.NONE);
    }

    @Override
    public Mono<HttpResponse> send(HttpRequest request, Context context) {
        final boolean eagerlyReadResponse = (boolean)context.getData("azure-eagerly-read-response").orElse(false);

        return Mono.fromCallable(() ->
            {
                BasicHttpEntityEnclosingRequest httpRequest;
                HttpHost httpHost;

                String method;
                String path;
                String query;

                try {
                    URI uri = request.getUrl().toURI();

                    method = request.getHttpMethod().toString();
                    path   = (uri.getPath() != null) ? uri.getPath() : "/";
                    query  = uri.getQuery();

                    if (query != null)
                        path += "?" + query;

                    httpRequest = new BasicHttpEntityEnclosingRequest(method, path, HTTP_1_1);

                    httpHost = new HttpHost(uri.getHost(), uri.getPort(), uri.getScheme());

                } catch (URISyntaxException e) {
                    throw LOGGER.logExceptionAsError(Exceptions.propagate(e));
                }

                final HttpHeaders headers = request.getHeaders();

                if (headers != null) {
                    for (HttpHeader header : headers) {
                        String headerName = header.getName();

                        if (!restrictedHeaders.contains(headerName.toLowerCase())) {
                            httpRequest.setHeader(headerName, header.getValue());
                        }
                    }
                }

                final String contentLength = headers.getValue("content-length");
                final String contentType   = headers.getValue("content-type");

                if (contentLength != null) {
                    long contentLengthLong = Long.parseLong(contentLength);

                    if (contentLengthLong > 0) {
                        // httpRequest.setHeader("content-length", contentLength);
                        httpRequest.setHeader("content-type",   contentType);

                        BodyInputStream isPipe =
                            new BodyInputStream(request.getBody());

                        httpRequest.setEntity(new InputStreamEntity(isPipe,
                                                                    contentLengthLong,
                                                                    ContentType.create(contentType)));
                    }
                }

                org.apache.http.HttpResponse httpResponse;

                try {
                    httpResponse = httpClient.execute(httpHost, httpRequest);
                } catch (IOException e) {
                    // throw logger.logExceptionAsError(Exceptions.propagate(e));
                    throw new RuntimeException(e);
                }

                return new ApacheHttpResponse(request, httpResponse);
            });
    }

    public void close() {
        if (httpClient != null) {
            try {
                httpClient.close();
            } catch (IOException e) {
            }
        }
    }

    class BodyInputStream extends PipedInputStream {

        class BodyInputThread extends Thread {

            @Override
            public void run() {
                try {
                    for (ByteBuffer byteBuffer : flux.toIterable()) {
                        if (stop)
                            return;

                        byte[] bytes = byteBuffer.array();

                        if (bytes.length > 0) {
                            osPipe.write(bytes, 0, bytes.length);
                            osPipe.flush();
                        }
                    }
                } catch (IOException e) {
                    LOGGER.error("io exception", e);
                }
            }
        }

        Thread            thread;
        PipedOutputStream osPipe;
        Flux<ByteBuffer>  flux;
        volatile boolean  stop;
        boolean           alreadyClosed;

        BodyInputStream(Flux<ByteBuffer> flux) throws IOException {
            super();

            this.flux   = flux;

            this.osPipe = new PipedOutputStream();
            this.osPipe.connect(this);

            this.thread = new BodyInputThread();
            this.thread.start();
        }

        @Override
        public synchronized void close() throws IOException {
            if (this.alreadyClosed)
                return;

            try {
                super.close();
            } finally {
                try {
                    if (osPipe != null) {
                        osPipe.close();
                        osPipe = null;
                    }
                } finally {
                    this.stop   = true;

                    this.thread.interrupt();
                    this.thread = null;

                    this.alreadyClosed = true;
                }
            }
        }
    }

    static HttpHeaders fromHttpHeaders(org.apache.http.HttpResponse response) {
        final HttpHeaders httpHeaders = new HttpHeaders();

        for (Header header : response.getAllHeaders())
            httpHeaders.put(header.getName(), header.getValue());

        return httpHeaders;
    }
}
