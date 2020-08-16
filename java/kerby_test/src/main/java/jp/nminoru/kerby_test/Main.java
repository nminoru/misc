package jp.nminoru.kerby_test;

import java.io.IOException;
import org.apache.kerby.kerberos.kerb.KrbException;
import org.apache.kerby.kerberos.kerb.server.KdcServer;
import org.apache.kerby.kerberos.kerb.server.SimpleKdcServer;
import org.apache.kerby.util.NetworkUtil;


/**
 *
 */
public class Main {
    public static final String    serverRealm = "EXAMPLE.COM";
    public static final String    serverHost  = "localhost";
    public static       int       serverPort  = -1;
    
    public static void main(String[] args) throws IOException, KrbException {
        KdcServer kdcServer;
        kdcServer = new SimpleKdcServer();
        
        kdcServer.setKdcRealm("EXAMPLE.COM");        
        kdcServer.setKdcHost(serverHost);
        kdcServer.setKdcPort(NetworkUtil.getServerPort());        
        kdcServer.setAllowUdp(false);
        kdcServer.setAllowTcp(true);

        kdcServer.init();
        kdcServer.start();

        System.out.println("Hit enter to stop it");
        System.in.read();
        
        kdcServer.stop();        
    }
}
    
