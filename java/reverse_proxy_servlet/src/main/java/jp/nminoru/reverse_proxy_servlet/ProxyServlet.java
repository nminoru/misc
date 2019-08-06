package jp.nminoru.reverse_proxy_servlet;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URI;
import java.net.URL;
import java.net.URISyntaxException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;
import javax.servlet.ServletException;
import javax.servlet.ServletInputStream;
import javax.servlet.ServletOutputStream;
import javax.servlet.http.HttpSession;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpUpgradeHandler;
import javax.servlet.http.WebConnection;
import javax.websocket.DeploymentException;
import org.apache.http.Header;
import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpHeaders;
import org.apache.http.HttpHost;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.NoHttpResponseException;
import org.apache.http.client.HttpClient;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.InputStreamEntity;
import org.apache.http.impl.NoConnectionReuseStrategy;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;
import org.apache.http.message.BasicHttpRequest;
import org.apache.http.message.BasicHttpEntityEnclosingRequest;
import org.apache.http.util.EntityUtils;
import lombok.extern.slf4j.Slf4j;

import jp.nminoru.reverse_proxy_servlet.WebSocketConnection;


@Slf4j
public class ProxyServlet extends HttpServlet {

    static final Pattern HeaderPattern = Pattern.compile("^(\\S+):(.+)$");

    final static Set<String> HopByHopHeaderSet =
        Arrays.asList(HttpHeaders.CONNECTION,
                      "Keep-Alive",
                      HttpHeaders.PROXY_AUTHENTICATE,
                      HttpHeaders.PROXY_AUTHORIZATION,
                      HttpHeaders.TE,
                      HttpHeaders.TRAILER,
                      HttpHeaders.TRANSFER_ENCODING,
                      HttpHeaders.UPGRADE)
        .stream().map(name -> name.toLowerCase()).collect(Collectors.toCollection(HashSet::new));

    private String hostHeader;

    public static class ProxyHttpUpgradeHandler implements HttpUpgradeHandler {
        private WebSocketConnection webSocketConnection;
        private String              host;
        private int                 port;
        private byte[]              request;

        public ProxyHttpUpgradeHandler() {
        }

        public void setWebSocketConnection(WebSocketConnection webSocketConnection) {
            this.webSocketConnection = webSocketConnection;
        }

        @Override
        public void init(WebConnection wc) {
            log.trace("ProxyHttpUpgradeHandler#init");

            try {
                webSocketConnection.setBothStreams(wc.getInputStream(), wc.getOutputStream());
                webSocketConnection.execute();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }

        @Override
        public void destroy() {
            log.trace("ProxyHttpUpgradeHandler#destroy");

            if (webSocketConnection != null)
                webSocketConnection.close();
        }
    }

    @Override
    protected void service(HttpServletRequest servletRequest, HttpServletResponse servletResponse) throws ServletException, IOException {
        String requestUri;
        String queryString = servletRequest.getQueryString();

        if (queryString != null)
            requestUri = servletRequest.getRequestURI() + "?" + queryString;
        else
            requestUri = servletRequest.getRequestURI();

        String method = servletRequest.getMethod();

        hostHeader = servletRequest.getHeader(HttpHeaders.HOST);

        log.trace("service: {} {} {}", method, requestUri, hostHeader);

        try {
            if (isUpgrade(method, servletRequest)) {
                URI targetUri = Main.TARGET_URI;

                URI path = new URI(null, null, null, 0,
                                   servletRequest.getRequestURI(),
                                   servletRequest.getQueryString(),
                                   null);

                serviceWebSocket(servletRequest, servletResponse, targetUri.getHost(), targetUri.getPort(), path.toASCIIString());
            } else {
                serviceNormalHttp(servletRequest, servletResponse, method, requestUri);
            }
        } catch (URISyntaxException e) {
            throw new IOException(e);
        }
    }

    private void serviceNormalHttp(HttpServletRequest servletRequest, HttpServletResponse servletResponse, String method, String requestUri) throws IOException {
        HttpHost httpHost = new HttpHost(Main.TARGET_URI.getHost(), Main.TARGET_URI.getPort(), Main.TARGET_URI.getScheme());

        try (CloseableHttpClient httpClient = HttpClientBuilder.create().disableRedirectHandling().setConnectionReuseStrategy(new NoConnectionReuseStrategy()).build()) {
            HttpRequest proxyRequest = getHttpRequest(method, requestUri, servletRequest);

            copyRequestHeaders(servletRequest, proxyRequest);

            if (hostHeader != null)
                proxyRequest.addHeader(HttpHeaders.HOST, hostHeader);

            HttpResponse proxyResponse = null;

            try {
                proxyResponse = httpClient.execute(httpHost, proxyRequest);

                int statusCode = proxyResponse.getStatusLine().getStatusCode();

                servletResponse.setStatus(statusCode);

                copyResponseHeaders(proxyResponse, servletRequest, servletResponse);

                if (statusCode == HttpStatus.SC_NOT_MODIFIED) {
                    servletResponse.setIntHeader(HttpHeaders.CONTENT_LENGTH, 0);
                } else {
                    copyResponseEntity(proxyResponse, servletResponse, proxyRequest, servletRequest);
                }
            } finally {
                if (proxyResponse != null)
                    EntityUtils.consumeQuietly(proxyResponse.getEntity());
            }
        }
    }

    private boolean isUpgrade(String method, HttpServletRequest servletRequest) {
        if ("GET".equals(method)) {
            String connectionHeader = servletRequest.getHeader(HttpHeaders.CONNECTION);

            if (connectionHeader != null) {
                for (String connectionPart : connectionHeader.split(",")) {
                    if ("Upgrade".equalsIgnoreCase(connectionPart.trim())) {
                        String upgradeHeader = servletRequest.getHeader(HttpHeaders.UPGRADE);

                        if (upgradeHeader != null) {
                            for (String upgradePart : upgradeHeader.split(",")) {
                                if ("WebSocket".equalsIgnoreCase(upgradePart.trim())) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    private void serviceWebSocket(HttpServletRequest servletRequest, HttpServletResponse servletResponse, String host, int port, String path) throws ServletException, IOException {
        log.trace("ProxyServlet#serviceWebSocket {}", path);

        WebSocketConnection webSocketConnection = new WebSocketConnection(host, port);

        webSocketConnection.sendRequest(getRequestHeader(servletRequest, path));
        setResponseHeader(servletResponse, webSocketConnection.receiveResponse());

        ProxyHttpUpgradeHandler handler = servletRequest.upgrade(ProxyHttpUpgradeHandler.class);
        handler.setWebSocketConnection(webSocketConnection);
    }

    public byte[] getRequestHeader(HttpServletRequest servletRequest, String path) {
        StringBuilder sb = new StringBuilder();

        sb.append("GET " + path + " HTTP/1.1\n");

        Enumeration<String> it1 = servletRequest.getHeaderNames();

        while (it1.hasMoreElements()) {
            String headerName = it1.nextElement();

            if (headerName.equalsIgnoreCase(HttpHeaders.CONTENT_LENGTH))
                continue;

            if (ProxyServlet.isHopByHopHeader(headerName))
                continue;

            Enumeration<String> it2 = servletRequest.getHeaders(headerName);

            List<String> headerValues = new ArrayList<>();

            while (it2.hasMoreElements()) {
                String headerValue = it2.nextElement();

                log.trace("header {} {}", headerName, headerValue);
                sb.append(headerName + ": " + headerValue + "\n");
            }
        }

        sb.append(HttpHeaders.CONNECTION + ": Upgrade\n");
        sb.append(HttpHeaders.UPGRADE    + ": websocket\n");
        sb.append("\n");

        return sb.toString().getBytes(StandardCharsets.UTF_8);
    }

    public void setResponseHeader(HttpServletResponse servletResponse, byte[] bytes) {
        String response = new String(bytes, StandardCharsets.UTF_8);

        String[] lines = response.split("\\r\\n");

        if (lines.length > 0) {
            String[] parts = lines[0].split("\\s+");

            if (parts.length >= 2)
                servletResponse.setStatus(Integer.parseInt(parts[1]));

            for (int i = 1 ; i < lines.length ; i++) {
                Matcher m = HeaderPattern.matcher(lines[i]);

                if (m.matches() && m.groupCount() == 2)
                    servletResponse.addHeader(m.group(1).trim(), m.group(2).trim());
            }
        }
    }

    private HttpRequest getHttpRequest(String method, String requestUri, HttpServletRequest servletRequest) throws IOException {
        if ((servletRequest.getContentLengthLong() > 0) || (servletRequest.getContentType() != null)) {
            String contentType = servletRequest.getContentType();

            HttpEntityEnclosingRequest enclosingRequest = new BasicHttpEntityEnclosingRequest(method, requestUri);
            enclosingRequest.setEntity(new InputStreamEntity(servletRequest.getInputStream(),
                                                             servletRequest.getContentLengthLong(),
                                                             ContentType.getByMimeType(contentType)));
            return enclosingRequest;
        } else {
            return new BasicHttpRequest(method, requestUri);
        }
    }

    private void copyRequestHeaders(HttpServletRequest servletRequest, HttpRequest proxyRequest) {
        Enumeration<String> it = servletRequest.getHeaderNames();

        while (it.hasMoreElements())
            copyRequestHeader(servletRequest, proxyRequest, it.nextElement());
    }

    private void copyRequestHeader(HttpServletRequest servletRequest, HttpRequest proxyRequest, String headerName) {
        // if (headerName.equalsIgnoreCase(HttpHeaders.HOST))
        //     return;

        if (headerName.equalsIgnoreCase(HttpHeaders.CONTENT_LENGTH))
            return;

        if (isHopByHopHeader(headerName))
            return;

        Enumeration<String> it = servletRequest.getHeaders(headerName);

        while (it.hasMoreElements()) {
            String headerValue = it.nextElement();

            proxyRequest.addHeader(headerName, headerValue);
        }
    }

    private void copyResponseHeaders(HttpResponse proxyResponse, HttpServletRequest servletRequest, HttpServletResponse servletResponse) {
        for (Header header : proxyResponse.getAllHeaders()) {
            copyResponseHeader(servletRequest, servletResponse, header);
        }
    }

    private void copyResponseHeader(HttpServletRequest servletRequest, HttpServletResponse servletResponse, Header header) {
        String headerName = header.getName();

        if (isHopByHopHeader(headerName))
            return;

        String headerValue = header.getValue();
        servletResponse.addHeader(headerName, headerValue);
    }

    private void copyResponseEntity(HttpResponse proxyResponse, HttpServletResponse servletResponse, HttpRequest proxyRequest, HttpServletRequest servletRequest) throws IOException {
        HttpEntity entity = proxyResponse.getEntity();

        if (entity != null)
            entity.writeTo(servletResponse.getOutputStream());
    }

    public static boolean isHopByHopHeader(String headerName) {
        return HopByHopHeaderSet.contains(headerName.toLowerCase());
    }
}
