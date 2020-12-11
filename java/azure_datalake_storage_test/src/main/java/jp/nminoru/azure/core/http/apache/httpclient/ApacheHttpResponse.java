package jp.nminoru.azure.core.http.apache.httpclient;

import java.io.InputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.List;

import com.azure.core.http.HttpRequest;
import com.azure.core.util.FluxUtil;
import com.azure.core.util.logging.ClientLogger;
import org.apache.http.Header;
import org.apache.http.HttpEntity;
import reactor.adapter.JdkFlowAdapter;
import reactor.core.Exceptions;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

import jp.nminoru.azure.core.http.apache.httpclient.ApacheHttpClient;


class ApacheHttpResponse extends ApacheHttpResponseBase {

    private final static ClientLogger LOGGER = new ClientLogger(ApacheHttpResponse.class);

    private final InputStream  inputStream;

    ApacheHttpResponse(final HttpRequest request, org.apache.http.HttpResponse response) {
        super(request, response.getStatusLine().getStatusCode(), ApacheHttpClient.fromHttpHeaders(response));

        this.inputStream = extractInputStream(response);
    }

    @Override
    public Flux<ByteBuffer> getBody() {
        if (this.inputStream == null)
            return Flux.empty();

        return FluxUtil
            .toFluxByteBuffer(this.inputStream)
            .doFinally(ignore ->
                       {
                           try {
                               this.inputStream.close();
                           } catch (IOException e) {
                               throw LOGGER.logExceptionAsError(Exceptions.propagate(e));
                           }
                       });
    }

    @Override
    public Mono<byte[]> getBodyAsByteArray() {
        return FluxUtil.collectBytesInByteBufferStream(getBody());
    }

    static InputStream extractInputStream(org.apache.http.HttpResponse response) {
        try {
            HttpEntity entity = response.getEntity();

            if (entity != null) {
                InputStream inputStream = entity.getContent();

                if (inputStream != null)
                    return inputStream;
            }
        } catch (IOException e) {
            LOGGER.error("io exception", e);
        }

        return null;
    }

    @Override
    public void close() {
        if (this.inputStream != null) {
            try {
                this.inputStream.close();
            } catch (IOException e) {
                LOGGER.warning("io exception", e);
            }
        }
    }
}
