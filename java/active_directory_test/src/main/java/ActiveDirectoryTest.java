/**
 * Active Directory の接続テスト
 *
 * @see
 * - <a href="https://www.earthlink.co.jp/engineerblog/intra-mart-engineerblog/3336/">JavaでActiveDirectory検索を行う(AD認証) | 株式会社アースリンク</a>
 * - <a href="https://github.com/hierynomus/smbj/issues/304">Kerberos support? Issue #304 - hierynomus/smbj</a>
 */
import java.io.IOException;
import java.security.PrivilegedActionException;
import java.security.PrivilegedExceptionAction;
import java.util.Arrays;
import java.util.Base64;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Map;
import java.util.Set;
import javax.naming.Context;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;
import javax.naming.directory.DirContext;
import javax.naming.directory.InitialDirContext;
import javax.naming.directory.SearchControls;
import javax.naming.directory.SearchResult;
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
import com.hierynomus.smbj.auth.GSSAuthenticationContext;
import com.hierynomus.msfscc.FileAttributes;
import com.hierynomus.msfscc.fileinformation.FileIdBothDirectoryInformation;
import com.hierynomus.protocol.commons.EnumWithValue;
import com.hierynomus.smbj.SMBClient;
import com.hierynomus.smbj.auth.AuthenticationContext;
import com.hierynomus.smbj.connection.Connection;
import com.hierynomus.smbj.session.Session;
import com.hierynomus.smbj.share.DiskShare;
import com.sun.security.auth.callback.TextCallbackHandler;
import com.sun.security.auth.module.Krb5LoginModule;
import com.sun.security.jgss.ExtendedGSSContext;
import com.sun.security.jgss.ExtendedGSSCredential;
import org.apache.commons.codec.binary.Hex;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;
import org.ietf.jgss.GSSManager;
import org.ietf.jgss.GSSName;
import org.ietf.jgss.Oid;


public class ActiveDirectoryTest {

    // Active Diretory の設定
    static String DomainName = "<AD domain name>";
    static String UserName = "<username>";
    static String Password = "<password>";
    static String LdapHostName = "<LDAP hostname>";
    static String KerberosHostName = "<Kerberos hostname>";

    // テスト用のSMBサーバー
    static String SmbHostName = "<SMB hostname>";
    static String ShareName = "<SMB share name>";

    public static void main(String[] args) throws Exception {
        DomainName       = System.getenv("SMB_DOMAIN");
        UserName         = System.getenv("SMB_USERNAME");
        Password         = System.getenv("SMB_PASSWORD");
        LdapHostName     = System.getenv("SMB_LDAP");
        KerberosHostName = System.getenv("SMB_KDC");
        SmbHostName      = System.getenv("SMB_HOST");
        ShareName        = System.getenv("SMB_SHARENAME");

        ActiveDirectoryTest activeDirectoryTest = new ActiveDirectoryTest();

        // Subject subject = activeDirectoryTest.login();

        // activeDirectoryTest.connectLdap();
        if (!activeDirectoryTest.connectKerberos()) {
            activeDirectoryTest.connectNTLM();
        }
    }

    void connectLdap() {
        Hashtable<String, String> env = new Hashtable<>();

        env.put(Context.INITIAL_CONTEXT_FACTORY, "com.sun.jndi.ldap.LdapCtxFactory");
        env.put(Context.SECURITY_AUTHENTICATION, "simple");
        env.put(Context.PROVIDER_URL,            "ldap://" + LdapHostName);
        env.put(Context.SECURITY_PRINCIPAL,      UserName + "@" + DomainName);
        env.put(Context.SECURITY_CREDENTIALS,    Password);

        // env.put(Context.SECURITY_PROTOCOL,       "ssl");
        // env.put(Context.REFERRAL,                "follow");
        env.put(Context.REFERRAL,                "ignore");

        DirContext dirContext = null;

        try {
            dirContext = new InitialDirContext(env);

            System.out.println("OK: connect LDAP");

            // TODO: ここで LDAP の検索を行う
        } catch (NamingException e) {
            e.printStackTrace();
        } finally {
            try {
                if (dirContext != null)
                    dirContext.close();
            } catch (NamingException e) {
                e.printStackTrace();
            }
        }
    }

    Subject serviceSubject;
    GSSCredential serviceCredentials;

    GSSCredential impersonate(Subject subject, final String someone) throws PrivilegedActionException {

        GSSCredential creds = Subject.doAs(subject,
            new PrivilegedExceptionAction<GSSCredential>() {

                @Override
                public GSSCredential run() throws Exception {
                    GSSManager manager = GSSManager.getInstance();

                    if (serviceCredentials == null)
                        serviceCredentials = manager.createCredential(GSSCredential.INITIATE_ONLY);

                    GSSName other = manager.createName(someone, GSSName.NT_USER_NAME);

                    return ((ExtendedGSSCredential)serviceCredentials).impersonate(other);
                }
            });

        return creds;
    }

    boolean connectKerberos() throws GSSException, IOException, LoginException, PrivilegedActionException {
        String realmName = DomainName.toUpperCase();

        System.out.println("realmName: " + realmName);

        System.setProperty("java.security.krb5.kdc",   KerberosHostName);
        System.setProperty("java.security.krb5.realm", realmName);
        System.setProperty("java.security.auth.login.config", "./jaas.conf");
        System.setProperty("javax.security.auth.useSubjectCredsOnly", "true");
        System.setProperty("java.security.krb5.debug", "true");
        System.setProperty("sun.security.krb5.debug",  "true");

        HashMap<String, String> loginParams = new HashMap<>();

        String principal = UserName + "@" + realmName;

        loginParams.put("principal", principal);

        LoginContext lc = new LoginContext("Client", null,
                                           new UserPasswordCallbackHandler(principal, Password),
                                           new CustomLoginConfiguration(loginParams));

        boolean success = false;

        try {
            System.out.println("Go Kerbros");
                
            lc.login();

            try {
                Subject subject = lc.getSubject();
                KerberosPrincipal krbPrincipal = subject.getPrincipals(KerberosPrincipal.class).iterator().next();

                GSSManager manager = GSSManager.getInstance();
                GSSName name = manager.createName(krbPrincipal.toString(), GSSName.NT_USER_NAME);
                Set<Oid> mechs = new HashSet<>(Arrays.asList(manager.getMechsForName(name.getStringNameType())));

                Oid mech;
                Oid spnegoOid = new Oid("1.3.6.1.5.5.2");
                Oid kerberos5Oid = new Oid("1.2.840.113554.1.2.2");

                System.out.println("enable support for kerbros: " + mechs.contains(kerberos5Oid));
                System.out.println("enable support for spnego: " + mechs.contains(spnegoOid));

                if (mechs.contains(spnegoOid)) {
                    mech = spnegoOid;
                } else if (mechs.contains(kerberos5Oid)) {
                    mech = kerberos5Oid;
                } else {
                    throw new IllegalArgumentException("No mechanism found");
                }
            
                GSSCredential credential = Subject.doAs(subject,
                    new PrivilegedExceptionAction<GSSCredential>() {
                        @Override
                        public GSSCredential run() throws GSSException {
                            return manager.createCredential(name, GSSCredential.DEFAULT_LIFETIME, mech, GSSCredential.INITIATE_ONLY);
                        }
                    });

                // TODO: credential から SPNEGO トークンを作ることができるはず。

                System.out.println("krb.principal: " + krbPrincipal.getName());
                System.out.println("krb.realm: " + krbPrincipal.getRealm());

                //
                // Kerberos を使ったアクセス
                //
                AuthenticationContext authCtx =
                    new GSSAuthenticationContext(krbPrincipal.getName(),
                                                 krbPrincipal.getRealm(),
                                                 subject,
                                                 credential);

                connectSmb(authCtx);
                
                // 成功
                success = true;
            } finally {
                lc.logout();
                System.out.println("Done Kerbros");                
            }
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }

         // 失敗
        return success;
    }

    String generateToken(ExtendedGSSContext context) throws Exception {
        byte[] token = new byte[0];

        token = context.initSecContext(token, 0, token.length);

        if (!context.isEstablished()) {
            System.err.println("Not established yet. Other rounds required.");
            // Refer to GssSpNegoClient.java code at
            // https://docs.oracle.com/javase/8/docs/technotes/guides/security/jgss/lab/part5.html
        }

        if (context.isEstablished()) {
            System.out.println("Context srcName " + context.getSrcName());
            System.out.println("Context targName " + context.getTargName());

        }

        String result = Base64.getEncoder().encodeToString(token);
        System.out.println("Token " + Base64.getEncoder().encodeToString(token));

        return result;
    }

    void connectSmb(AuthenticationContext ac) throws IOException {

        //
        // ユーザ・パスワードを与える場合は以下のようなコードになる
        //
        // AuthenticationContext ac =
        //     new AuthenticationContext(userName, password.toCharArray(), domainName);

        SMBClient client = new SMBClient();
        try (Connection connection = client.connect(SmbHostName)) {
            Session session = connection.authenticate(ac);

            try (DiskShare share = (DiskShare) session.connectShare(ShareName)) {
                for (FileIdBothDirectoryInformation f : share.list("", "*")) {
                    long fileAttributes = f.getFileAttributes();

                    System.out.println("File : " + f.getFileName());
                    System.out.println("\t" + f.getCreationTime());
                    System.out.println("\t" + f.getLastAccessTime());
                    System.out.println("\t" + f.getLastWriteTime());
                    System.out.println("\t" + f.getChangeTime());
                    System.out.println("\t" + f.getAllocationSize());
                    System.out.println("\t" + fileAttributes);

                    if (EnumWithValue.EnumUtils.isSet(fileAttributes, FileAttributes.FILE_ATTRIBUTE_DIRECTORY))
                        System.out.println("\tDIRECTORY");

                    if (EnumWithValue.EnumUtils.isSet(fileAttributes, FileAttributes.FILE_ATTRIBUTE_HIDDEN))
                        System.out.println("\tHIDDEN");

                    System.out.println("\t" + f.getEaSize());
                    System.out.println("\t" +  new String(Hex.encodeHex(f.getFileId())));
                }
            }
        }
    }

    void connectNTLM() throws IOException {
        System.out.println("Go NTLM");

        AuthenticationContext authCtx =
            new AuthenticationContext(UserName, Password.toCharArray(), DomainName);

        connectSmb(authCtx);

        System.out.println("Done NTLM");
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
}
