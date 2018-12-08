package jp.nminoru.smbj_test;

import java.io.IOException;
import com.hierynomus.msfscc.FileAttributes;
import com.hierynomus.msfscc.fileinformation.FileIdBothDirectoryInformation;
import com.hierynomus.smbj.SMBClient;
import com.hierynomus.smbj.auth.AuthenticationContext;
import com.hierynomus.smbj.connection.Connection;
import com.hierynomus.smbj.session.Session;
import com.hierynomus.smbj.share.DiskShare;

import org.apache.commons.codec.binary.Hex;

public class Main {
    final static String ServerName = "SERVER";
    final static String DomainName = "DOMAIN";
    final static String UserName   = "USERNAME";
    final static String Password   = "PASSWORD";
    final static String ShareName  = "SHARENAME";
    
    public static void main(String[] args) throws IOException {
        SMBClient client = new SMBClient();

        try (Connection connection = client.connect(ServerName)) {
            AuthenticationContext ac = new AuthenticationContext(UserName, Password.toCharArray(), DomainName);
            Session session = connection.authenticate(ac);

            // Connect to Share
            try (DiskShare share = (DiskShare) session.connectShare(ShareName)) {
                for (FileIdBothDirectoryInformation f : share.list("*", "*")) {
                    long fileAttributes = f.getFileAttributes();
                    
                    System.out.println("File : " + f.getFileName());
                    System.out.println("\t" + f.getCreationTime());
                    System.out.println("\t" + f.getLastAccessTime());
                    System.out.println("\t" + f.getLastWriteTime());
                    System.out.println("\t" + f.getChangeTime());
                    System.out.println("\t" + f.getAllocationSize());
                    System.out.println("\t" + fileAttributes);
                    
                    if ((fileAttributes & FileAttributes.FILE_ATTRIBUTE_DIRECTORY.getValue()) != 0)
                        System.out.println("\tDIRECTORY");

                    System.out.println("\t" + f.getEaSize());
                    System.out.println("\t" +  new String(Hex.encodeHex(f.getFileId())));
                }
            }
        }
    }
}
