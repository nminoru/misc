package jp.nminoru.azure.core.http.apache.httpclient;

import java.nio.ByteBuffer;

import com.azure.core.http.HttpHeaders;
import com.azure.core.http.HttpRequest;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;


class BufferedApacheHttpResponse extends ApacheHttpResponseBase {
    private final byte[] body;

    BufferedApacheHttpResponse(HttpRequest request, int statusCode, HttpHeaders headers, byte[] body) {
        super(request, statusCode, headers);

        this.body = body;
    }

    @Override
    public Flux<ByteBuffer> getBody() {
        return Flux.defer(() -> Flux.just(ByteBuffer.wrap(body)));
    }

    @Override
    public Mono<byte[]> getBodyAsByteArray() {
        return Mono.defer(() -> Mono.just(body));
    }
}
