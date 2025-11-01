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
import javax.naming.AuthenticationException;
import javax.naming.Context;
import javax.naming.NamingEnumeration;
import javax.naming.NamingException;
import javax.naming.directory.Attributes;
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
import javax.security.auth.login.FailedLoginException;
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
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;
import org.ietf.jgss.GSSManager;
import org.ietf.jgss.GSSName;
import org.ietf.jgss.Oid;


public class ActiveDirectoryTest {

    enum Method {
	Smb,
	Ldap
    };

    // オプションから与えられる値
    static String DomainName       = "<AD domain name>";
    static String UserName         = "<username>";
    static String Password         = "<password>";
    static String LdapHostName     = "<LDAP hostname>";
    static String KerberosHostName = "<Kerberos hostname>";
    static String UserDn           = "<user dn>";
    static String Filter           = "<filter>";

    // テスト用のSMBサーバー
    static String SmbHostName      = "<SMB hostname>";
    static String ShareName        = "<SMB share name>";

    public static void main(String[] args) throws Exception {
        DomainName       = System.getenv("ADT_DOMAIN");
        UserName         = System.getenv("ADT_USERNAME");
        Password         = System.getenv("ADT_PASSWORD");
        LdapHostName     = System.getenv("ADT_LDAP");
        KerberosHostName = System.getenv("ADT_KDC");

        SmbHostName      = System.getenv("ADT_SMB_HOST");
        ShareName        = System.getenv("ADT_SMB_SHARENAME");

	UserDn           = System.getenv("ADT_LDAP_USER_DN");
	Filter           = System.getenv("ADT_LDAP_FILTER");

        ActiveDirectoryTest activeDirectoryTest = new ActiveDirectoryTest();

        if (args.length > 0) {
            switch (args[0]) {
	        case "kerberos+smb":
                    activeDirectoryTest.connectKerberos(Method.Smb);
                    return;
	        case "kerberos+ldap":
                    activeDirectoryTest.connectKerberos(Method.Ldap);
                    return;
	        case "ntlm":
                    activeDirectoryTest.connectNTLM();
                    return;
                case "ldap":
                    activeDirectoryTest.connectLdap();
                    return;
                default:
                    break;
            }
        }

        System.out.println("Usage kerberos|ntlm|ldap");
    }

    void connectLdap() {
        Hashtable<String, String> env = new Hashtable<>();

        env.put(Context.INITIAL_CONTEXT_FACTORY, "com.sun.jndi.ldap.LdapCtxFactory");
        env.put(Context.SECURITY_AUTHENTICATION, "simple");
        env.put(Context.PROVIDER_URL,            "ldap://" + LdapHostName);
        env.put(Context.SECURITY_PRINCIPAL,      UserName + "@" + DomainName);
        env.put(Context.SECURITY_CREDENTIALS,    Password);
        env.put("java.naming.ldap.attributes.binary", "objectSid");

	// LDAP署名対応
	env.put("javax.security.sasl.qop",       "auth-int");

        // env.put(Context.SECURITY_PROTOCOL,       "ssl");
        // env.put(Context.REFERRAL,                "follow");
        env.put(Context.REFERRAL,                "ignore");

        DirContext dirContext = null;

        try {
            dirContext = new InitialDirContext(env);

	    System.out.println();
            System.out.println("OK: connect LDAP");

	    // CN 検索
            // attrs = search("cn=" + searchUserName);
	    SearchControls constraints = new SearchControls();
	    constraints.setSearchScope(SearchControls.SUBTREE_SCOPE);

	    NamingEnumeration<SearchResult> results = null;
	    try {
		results = dirContext.search(UserDn, Filter, constraints);
	    } finally {
		if (results != null) {
		    while (results.hasMore()) {
			SearchResult sr = results.next();
			System.out.println();
			System.out.println("SearchResult: " + sr.toString());
		    }

		    try {
			results.close();
		    } catch (NamingException e) {
			System.out.println("Failed to close connection to LDAP server");
			e.printStackTrace(System.err);
		    }
		}
	    }
        } catch (AuthenticationException e) {
            // ユーザー認証失敗
            System.err.println("# User authentication failed");
            e.printStackTrace(System.err);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        } finally {
            try {
                if (dirContext != null)
                    dirContext.close();
            } catch (NamingException e) {
                e.printStackTrace();
            }
        }
    }

    void connectLdapViaKerberos() {
        Hashtable<String, String> env = new Hashtable<>();

        env.put(Context.INITIAL_CONTEXT_FACTORY, "com.sun.jndi.ldap.LdapCtxFactory");
        env.put(Context.PROVIDER_URL,            "ldap://" + LdapHostName);
        env.put(Context.REFERRAL,                "follow");
        env.put("java.naming.ldap.attributes.binary", "objectSid");

	// LDAP署名対応
        env.put("javax.security.sasl.qop",       "auth-int");

        // env.put(Context.SECURITY_PRINCIPAL,      UserName + "@" + DomainName);
        // env.put(Context.SECURITY_CREDENTIALS,    Password);

        env.put(Context.SECURITY_AUTHENTICATION, "GSSAPI");
        env.put("javax.security.sasl.server.authentication", "true");

        DirContext dirContext = null;

        try {
            dirContext = new InitialDirContext(env);

	    System.out.println();
            System.out.println("OK: connect LDAP");

	    // CN 検索
            // attrs = search("cn=" + searchUserName);

	    SearchControls constraints = new SearchControls();
	    constraints.setSearchScope(SearchControls.SUBTREE_SCOPE);

	    NamingEnumeration<SearchResult> results = null;
	    try {
		results = dirContext.search(UserDn, Filter, constraints);
	    } finally {
		if (results != null) {
		    while (results.hasMore()) {
			SearchResult sr = results.next();
			System.out.println();
			System.out.println("SearchResult: " + sr.toString());
		    }

		    try {
			results.close();
		    } catch (NamingException e) {
			System.out.println("Failed to close connection to LDAP server");
			e.printStackTrace(System.err);
		    }
		}
	    }
        } catch (AuthenticationException e) {
            // ユーザー認証失敗
            System.err.println("# User authentication failed");
            e.printStackTrace(System.err);
        } catch (Exception e) {
            e.printStackTrace(System.err);
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

    boolean connectKerberos(Method method) throws GSSException, IOException, LoginException, PrivilegedActionException {
        String realmName = DomainName.toUpperCase();

	System.out.println();
        System.out.println("realmName: " + realmName);

        System.setProperty("java.security.krb5.kdc",   KerberosHostName);
        System.setProperty("java.security.krb5.realm", realmName);
        // System.setProperty("java.security.auth.login.config", "./jaas.conf");
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
	    System.out.println();
            System.out.println("Go Kerbros");
	    System.out.println();

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
		switch (method) {

		case Smb:
		    {
			AuthenticationContext authCtx =
			    new GSSAuthenticationContext(krbPrincipal.getName(),
							 krbPrincipal.getRealm(),
							 subject,
							 credential);
			connectSmb(authCtx);
		    }
		    break;

		case Ldap:
		    {
			Subject.doAs(subject,
				     new PrivilegedExceptionAction<Void>() {
					 @Override
					 public Void run() throws GSSException {
					     connectLdapViaKerberos();
					     return (Void)null;
					 }
				     });
		    }
		    break;
		}

                // 成功
                success = true;
            } finally {
                lc.logout();
                System.out.println("Done Kerbros");
		System.out.println();
            }
        } catch (FailedLoginException e) {
            // ユーザー認証失敗
            System.err.println("# User authentication failed");
            e.printStackTrace(System.err);
        } catch (LoginException e) {
            String message = e.getMessage();
            if (message.startsWith("Pre-authentication information was invalid")) {
                System.err.println("# User authentication failed");
            }
            e.printStackTrace(System.err);
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

    void connectNTLM() throws IOException {
        System.out.println("Go NTLM");

        AuthenticationContext authCtx =
            new AuthenticationContext(UserName, Password.toCharArray(), DomainName);

        try {
            connectSmb(authCtx);
        } catch (Exception e) {
            e.printStackTrace(System.err);
        }

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
                }
            }
        }
    }
}
