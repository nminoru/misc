package jp.nminoru.azure.core.http.apache.httpclient;

import java.nio.charset.Charset;

import com.azure.core.http.HttpHeaders;
import com.azure.core.http.HttpRequest;
import com.azure.core.http.HttpResponse;
import com.azure.core.util.CoreUtils;
import reactor.core.publisher.Mono;


abstract class ApacheHttpResponseBase extends HttpResponse {
    private final int statusCode;
    private final HttpHeaders headers;

    ApacheHttpResponseBase(HttpRequest request, int statusCode, HttpHeaders headers) {
        super(request);

        this.statusCode = statusCode;
        this.headers = headers;
    }

    @Override
    public int getStatusCode() {
        return this.statusCode;
    }

    @Override
    public String getHeaderValue(String name) {
        return this.headers.getValue(name);
    }

    @Override
    public HttpHeaders getHeaders() {
        return this.headers;
    }

    @Override
    public Mono<String> getBodyAsString() {
        return getBodyAsByteArray().map(bytes -> CoreUtils.bomAwareToString(bytes, getHeaderValue("Content-Type")));
    }

    @Override
    public Mono<String> getBodyAsString(Charset charset) {
        return getBodyAsByteArray().map(bytes -> new String(bytes, charset));
    }
}
