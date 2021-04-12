package jp.nminoru.socks_proxy_test;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.net.HttpURLConnection;
import java.net.URL;


public class Test {

    public static final String urlString     = "https://www.google.com/";
    public static final String socksHost     = "(host)";
    public static final int    socksPort     = 1080;
    public static final String socksUser     = "(user)";
    public static final String socksPassword = "(password)";

    public static void main(String[] args) {
        MyProxySelector.init();
        MyAuthenticator.init();

		HttpURLConnection  urlConn = null;

		try {
			//接続するURLを指定する
			URL url = new URL(urlString);

			//コネクションを取得する
			urlConn = (HttpURLConnection)url.openConnection();

			urlConn.setRequestMethod("GET");
			urlConn.connect();

			int status = urlConn.getResponseCode();

		    if (status == HttpURLConnection.HTTP_OK) {
                try (InputStream in = urlConn.getInputStream()) {
                    try (BufferedReader reader = new BufferedReader(new InputStreamReader(in))) {
                        StringBuilder output = new StringBuilder();
                        String line;

                        while ((line = reader.readLine()) != null) {
                            output.append(line);
                        }

                        System.out.println(output.toString());
                    }
                }
            }
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
            if (urlConn != null)
                urlConn.disconnect();
		}
	}
}
