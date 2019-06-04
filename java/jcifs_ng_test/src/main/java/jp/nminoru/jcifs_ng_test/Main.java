package jp.nminoru.jcifs_ng_test;

import java.net.MalformedURLException;
import java.util.Properties;
import javax.security.auth.Subject;
import javax.security.auth.kerberos.KerberosPrincipal;
import jcifs.CIFSContext;
import jcifs.CIFSException;
import jcifs.config.PropertyConfiguration;
import jcifs.context.BaseContext;
import jcifs.smb.Kerb5Authenticator;
import jcifs.smb.NtlmPasswordAuthenticator;
import jcifs.smb.SmbException;
import jcifs.smb.SmbFile;


public class Main {

    static boolean useKerberos = true;
    
    public static void main(String[] args) throws CIFSException, MalformedURLException, SmbException {
        String url = getEnv("SMB_URL");
        String domainName = getEnv("SMB_DOMAIN");
        String userName   = getEnv("SMB_USERNAME");
        String password   = getEnv("SMB_PASSWORD");

        Properties props = new Properties();
        CIFSContext context = new BaseContext(new PropertyConfiguration(props));

        if (useKerberos) {
            String realm = domainName.toUpperCase();
            
            // Kerb5Authenticator
            KerberosPrincipal principal = new KerberosPrincipal(String.format("%s@%s", userName, realm), KerberosPrincipal.KRB_NT_PRINCIPAL);
            Subject subject = null; // @todo 
            context.withCredentials(new Kerb5Authenticator(subject, password, realm, null /* expire */));
        } else {
            // NtlmPasswordAuthenticator
            context.withCredentials(new NtlmPasswordAuthenticator(domainName, userName, password));
        }

        // "smb://server/sharename/"
        SmbFile smbFile = new SmbFile(url, context.withCredentials(new NtlmPasswordAuthenticator(domainName, userName, password)));

        for (SmbFile childFile : smbFile.listFiles()) {
            System.out.print("name: " + childFile.getName());
        }
    }

    public static String getEnv(String name) {
        String value = System.getenv(name);

        if (value == null) {
            System.err.println(name + " is not defiend");
            System.exit(1);
        }

        return value;
    }    
}
