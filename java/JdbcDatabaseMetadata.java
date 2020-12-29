import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class JdbcDatabaseMetadata {
    
    public static void main(String[] args) throws Exception {

        try (Connection connection = DriverManager.getConnection(args[0])) {
            DatabaseMetaData dmd = connection.getMetaData();
            
            System.out.println("catalog: "               + dmd.getCatalogTerm());
            System.out.println("schema: "                + dmd.getSchemaTerm());
            System.out.println("isCatalogAtStart: "      + dmd.isCatalogAtStart());
            System.out.println("CatalogSeparator: "      + dmd.getCatalogSeparator());
            System.out.println("IdentifierQuoteString: " + dmd.getIdentifierQuoteString());        
        }
    }
}
