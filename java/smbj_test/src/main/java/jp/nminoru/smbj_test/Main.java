package jp.nminoru.smbj_test;

import java.io.IOException;
import java.nio.ByteBuffer;
import com.hierynomus.msfscc.FileAttributes;
import com.hierynomus.msfscc.fileinformation.FileIdBothDirectoryInformation;
import com.hierynomus.protocol.commons.EnumWithValue;
import com.hierynomus.smbj.SMBClient;
import com.hierynomus.smbj.auth.AuthenticationContext;
import com.hierynomus.smbj.connection.Connection;
import com.hierynomus.smbj.session.Session;
import com.hierynomus.smbj.share.DiskShare;

import org.apache.commons.codec.binary.Hex;

public class Main {
    
    public static void main(String[] args) throws IOException {
        String serverName = getEnv("SMB_SERVER");
        String domainName = getEnv("SMB_DOMAIN");
        String userName   = getEnv("SMB_USERNAME");
        String password   = getEnv("SMB_PASSWORD");
        String shareName  = getEnv("SMB_SHARENAME");

        SMBClient client = new SMBClient();
        
        try (Connection connection = client.connect(serverName)) {
            AuthenticationContext ac = new AuthenticationContext(userName, password.toCharArray(), domainName);
            Session session = connection.authenticate(ac);

            // Connect to Share
            try (DiskShare share = (DiskShare) session.connectShare(shareName)) {
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
                    long fileId = f.getFileId();
                    byte[] bytes = ByteBuffer.allocate(4).putLong(fileId).array();
                    System.out.println("\t" +  new String(Hex.encodeHex(bytes)));
                }
            }
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
