package jp.nminoru.socks_proxy_test;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.Proxy;
import java.net.ProxySelector;
import java.net.URI;
import java.util.Arrays;
import java.util.List;
import lombok.extern.slf4j.Slf4j;


@Slf4j
public class MyProxySelector extends ProxySelector {

    private ProxySelector proxySelector;

    public MyProxySelector(ProxySelector proxySelector) {
        this.proxySelector = proxySelector;
    }

    @Override    
    public void connectFailed(URI uri, SocketAddress sa, IOException ioe) {
        proxySelector.connectFailed(uri, sa, ioe);
    }

    @Override
    public List<Proxy> select(URI uri) {
        log.info("MyProxySelector#select {}", uri.toASCIIString());

        if ("https".equals(uri.getScheme()) && "www.google.com".equals(uri.getHost())) {
            Proxy proxy = new Proxy(Proxy.Type.SOCKS, InetSocketAddress.createUnresolved(Test.socksHost, Test.socksPort));
            return Arrays.asList(proxy);            
        }
        
        return proxySelector.select(uri);
    }

    public static void init() {
        MyProxySelector myProxySelector = new MyProxySelector(ProxySelector.getDefault());
        ProxySelector.setDefault(myProxySelector);
    }
}
