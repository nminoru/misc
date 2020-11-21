package jp.nminoru.spnego_test;

import java.io.InputStream;
import java.io.IOException;
import java.net.URI;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.Base64;
import java.util.Base64;
import java.security.PrivilegedActionException;
import java.security.PrivilegedExceptionAction;
import javax.security.sasl.AuthenticationException;
import javax.security.auth.Subject;
import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;
import javax.security.auth.kerberos.KerberosPrincipal;
import javax.security.auth.login.AppConfigurationEntry;
import javax.security.auth.login.Configuration;
import javax.security.auth.login.LoginContext;
import javax.security.auth.login.LoginException;
import org.apache.http.Header;
import org.apache.http.HttpException;
import org.apache.http.HttpHost;
import org.apache.http.HttpRequest;
import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.auth.AuthScheme;
import org.apache.http.auth.AuthSchemeProvider;
import org.apache.http.auth.AuthScope;
import org.apache.http.auth.Credentials;
import org.apache.http.client.CredentialsProvider;
import org.apache.http.client.config.AuthSchemes;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.config.Registry;
import org.apache.http.config.RegistryBuilder;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.NoConnectionReuseStrategy;
import org.apache.http.impl.auth.SPNegoScheme;
import org.apache.http.impl.auth.SPNegoSchemeFactory;
import org.apache.http.impl.client.BasicCredentialsProvider;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.impl.client.HttpClientBuilder;
import org.ietf.jgss.GSSContext;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;
import org.ietf.jgss.GSSManager;
import org.ietf.jgss.GSSName;
import org.ietf.jgss.Oid;


public class Main {

    static final Base64.Decoder decoder = Base64.getDecoder();
    static final Base64.Encoder encoder = Base64.getEncoder();

    static final Oid GSS_KRB5_MECH_OID = createOid("1.2.840.113554.1.2.2");
    static final Oid SPNEGO_OID        = createOid("1.3.6.1.5.5.2");

    static String DomainName       = "<domain name>";
    static String UserName         = "<user name>";
    static String Password         = "<password>";
    static String KerberosHostName = "<kdc host>";
    static String Url              = "<http url>";
    static String ProxyUrl         = "<http url>";

    static Oid createOid(String oidString) {
        try {
            return new Oid(oidString);
        } catch (GSSException e) {
            throw new RuntimeException(e);
        }
    }

    public static void main(String[] args) throws Exception {
        DomainName       = System.getenv("SPNEGO_TEST_DOMAIN");
        UserName         = System.getenv("SPNEGO_TEST_USER");
        Password         = System.getenv("SPNEGO_TEST_PASSWORD");
        KerberosHostName = System.getenv("SPNEGO_TEST_KDC");
        Url              = System.getenv("SPNEGO_TEST_URL");
        ProxyUrl         = System.getenv("SPNEGO_TEST_PROXYURL");

        System.setProperty("java.security.krb5.kdc",                  KerberosHostName);
        System.setProperty("java.security.krb5.realm",                DomainName.toUpperCase());
        System.setProperty("javax.security.auth.useSubjectCredsOnly", "false");
        System.setProperty("java.security.krb5.debug",                "true");
        System.setProperty("sun.security.krb5.debug",                 "true");
        System.setProperty("sun.security.spnego.debug",               "true");

        String realmName = DomainName.toUpperCase();

        Main main = new Main();

        main.userPrincipal = UserName + "@" + realmName;
        main.uri = URI.create(Url);

        main.servicePrincipal = "HTTP@" + main.uri.getHost();

        main.run();
    }

    String userPrincipal;
    String servicePrincipal;
    URI    uri;

    public void run() throws LoginException {
        HashMap<String, String> loginParams = new HashMap<>();

        loginParams.put("principal", userPrincipal);

        LoginContext lc = null;

        try {
            lc = new LoginContext("Client", null,
                             new UserPasswordCallbackHandler(userPrincipal, Password),
                             new CustomLoginConfiguration(loginParams));
        } catch (SecurityException e) {
            e.printStackTrace(System.err);
            return;
        }

        lc.login();

        try {
            Subject subject = lc.getSubject();

            Subject.doAs(subject,
                new PrivilegedExceptionAction<Void>() {

                    @Override
                    public Void run() throws GSSException {
                        runrun1();

                        return (Void)null;
                    }
                });
        } catch (PrivilegedActionException e) {
            e.printStackTrace(System.err);
        } finally {
            lc.logout();
        }
    }

    void runrun1() throws GSSException {
        GSSContext gssContext = null;

        try {
            GSSManager gssManager = GSSManager.getInstance();
            GSSName    serviceName;

            serviceName = gssManager.createName(servicePrincipal,
                                                GSSName.NT_HOSTBASED_SERVICE);

            gssContext  = gssManager.createContext(serviceName,
                                                   // GSS_KRB5_MECH_OID,
                                                   SPNEGO_OID,
                                                   null,
                                                   GSSContext.DEFAULT_LIFETIME);
            gssContext.requestCredDeleg(true);
            gssContext.requestMutualAuth(true);

            byte[]  inToken = new byte[0];
            byte[]  outToken;
            boolean established = false;

            int count = 0;

         loop:
            while (!established) {
                System.out.println("### STEP (count: " + count + ") ###");

                outToken = gssContext.initSecContext(inToken, 0, inToken.length);

                try (CloseableHttpClient httpClient =
                     HttpClientBuilder.create().disableRedirectHandling().setConnectionReuseStrategy(new NoConnectionReuseStrategy()).build()) {

                    HttpGet httpGet;

                    if (ProxyUrl != null) {
                        httpGet = new HttpGet(ProxyUrl);
                        httpGet.addHeader("Host",  uri.getHost() + ":" + (uri.getPort() > 0 ? uri.getPort() : 80));
                    } else {
                        httpGet = new HttpGet(Url);
                    }

                    httpGet.addHeader("Authorization", "Negotiate "  + encoder.encodeToString(outToken));

                    try (CloseableHttpResponse httpResponse = httpClient.execute(httpGet)) {
                        printResponse(httpResponse);

                        if (gssContext.isEstablished()) {
                            established = true;
                            break loop;
                        }

                        int status = httpResponse.getStatusLine().getStatusCode();

                        if (status == HttpStatus.SC_UNAUTHORIZED) {
                            Header authHeader = httpResponse.getFirstHeader("WWW-Authenticate");

                            if (authHeader != null) {
                                String negotiate  = extractAuthHeader(authHeader.getValue());

                                if (negotiate == null)
                                    throw new AuthenticationException("Invalid SPNEGO sequence, 'WWW-Authenticate' header incorrect: " + authHeader.getValue());

                                inToken = decoder.decode(negotiate);
                            } else {
                                System.out.println("No WWW-Authenticate");
                                established = true;
                                break loop;
                            }
                        } else {
                            break loop;
                        }
                    }
                }

                count++;
            }
        } catch (IOException e) {
            e.printStackTrace(System.err);
        } finally {
            if (gssContext != null) {
                gssContext.dispose();
                gssContext = null;
            }
        }
    }

    static class UserPasswordCallbackHandler implements CallbackHandler {
        private String username;
        private String password;

        public UserPasswordCallbackHandler(String username, String password) {
            this.username = username;
            this.password = password;
        }

        @Override
        public void handle(Callback[] callbacks) throws IOException, UnsupportedCallbackException {
            for (Callback callback : callbacks) {
                if (callback instanceof NameCallback) {
                    NameCallback nameCallback = (NameCallback)callback;
                    nameCallback.setName(username);
                } else if (callback instanceof PasswordCallback) {
                    PasswordCallback passwordCallback = (PasswordCallback)callback;
                    passwordCallback.setPassword(password.toCharArray());
                } else {
                    throw new UnsupportedCallbackException(callback, "Unrecognised callback");
                }
            }
        }
    }

    static class CustomLoginConfiguration extends Configuration {

        final Map<String, String> params = new HashMap<>();

        public CustomLoginConfiguration(Map<String, String> params) {
            this.params.putAll(params);
        }

        @Override
        public AppConfigurationEntry[] getAppConfigurationEntry(String name) {
            AppConfigurationEntry configEntry =
                new AppConfigurationEntry("com.sun.security.auth.module.Krb5LoginModule",
                                          AppConfigurationEntry.LoginModuleControlFlag.REQUIRED,
                                          params);

            return new AppConfigurationEntry[]{configEntry};
        }
    }

    static String extractAuthHeader(String authHeader) {
        if (authHeader == null)
            return null;

        authHeader = authHeader.trim();

        String[] parts = authHeader.split("\\s+");

        if (parts.length != 2)
            return null;

        if (!parts[0].equalsIgnoreCase("Negotiate"))
            return null;

        return parts[1];
    }

    static void printResponse(HttpResponse httpResponse) throws IOException {
        System.out.println("status: " + httpResponse.getStatusLine().getStatusCode());

        for (Header header : httpResponse.getAllHeaders()) {
            System.out.println(header.getName() + ": " + header.getValue());
        }

        byte[] bytes = new byte[4096];

        int nbytes;
        InputStream inputStream = httpResponse.getEntity().getContent();

        while ((nbytes = inputStream.read(bytes)) > 0) {
            System.out.write(bytes, 0, nbytes);
        }

        System.out.println();
    }
}
