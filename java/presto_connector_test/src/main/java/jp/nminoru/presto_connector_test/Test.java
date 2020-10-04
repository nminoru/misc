package jp.nminoru.presto_connector_test;

import java.util.Properties;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import com.facebook.presto.jdbc.PrestoDriver;


public class Test {
    
    public static void main(String[] args) throws Exception {
        String     url = "jdbc:presto://(host):(port)/";
        Properties properties = new Properties();
        
        properties.setProperty("user",     "(user)");
        properties.setProperty("password", "(password)");
        properties.setProperty("SSL",      "true");

        // properties.setProperty("SSLTrustStorePath",     "(keystore.jks)");
        // properties.setProperty("SSLTrustStorePassword", "changeit");        

        PrestoDriver prestoDriver = new PrestoDriver();
        
        try (Connection conn = prestoDriver.connect(url, properties)) {
            DatabaseMetaData dmd = conn.getMetaData();

            try (ResultSet rs = dmd.getCatalogs()) {
                while (rs.next()) {                
                    String catalogName = rs.getString(1);

                    System.out.println("catalogName: " + catalogName);
                }
            }

            conn.close();
        }
    }
}
