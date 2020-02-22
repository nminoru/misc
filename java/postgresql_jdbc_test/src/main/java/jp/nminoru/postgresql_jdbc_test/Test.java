package jp.nminoru.postgresql_jdbc_test;

import java.sql.Array;
import java.sql.PreparedStatement;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.Statement;
import java.sql.SQLException;
import java.sql.Types;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.UUID;
import org.postgresql.util.PGobject;


public class Test {

    public static final int SIZE = 10;
    
    public static void main(String[] agrs) throws ClassNotFoundException, SQLException {
        Connection connection;
        
        Class.forName("org.postgresql.Driver");
        
        Properties info = new Properties();

        info.setProperty("user",     "<user>");
        info.setProperty("password", "<password>");
        
        connection = DriverManager.getConnection("jdbc:postgresql://127.0.0.1:5432/<DBテーブル>", info);
        connection.setAutoCommit(true);

        Object[] in = new Object[SIZE];

        for (int i = 0 ; i < SIZE ; i++) {
            PGobject cc = new PGobject();
            
            UUID uuid    = UUID.randomUUID();
            long version = i;
            
            cc.setType("entity_key");
            cc.setValue("(" + uuid.toString() + "," + String.valueOf(version) + ")");

            in[i] = cc;
        }        
        
        try (PreparedStatement pstmt = connection.prepareStatement("INSERT INTO entity_table (guid, version) SELECT E.guid, E.version FROM unnest(?::entity_key[]) E;")) {
            Array array = connection.createArrayOf("entity_key", in);
            pstmt.setArray(1, array);
            
            // pstmt.setObject(1, uuid, Types.OTHER);
            // pstmt.setLong   (2, 123L);

            pstmt.executeUpdate();
        }
    }
}
