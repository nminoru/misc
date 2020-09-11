import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;


public class Test {

    public static String jdbcUrl      = "jdbc:marklogic://localhost:8077/";
    public static String jdbcUser     = "user";
    public static String jdbcPassword = "password";
    
    public static void main(String[] args) throws Exception {
        jdbcUrl      = System.getenv("MARKLOGIC_URL");
        jdbcUser     = System.getenv("MARKLOGIC_USER");
        jdbcPassword = System.getenv("MARKLOGIC_PASSWORD");
        
        Class<?> myClass = Class.forName("com.marklogic.Driver");
        
        DriverManager.setLogStream(System.out);
        
        try (Connection conn = DriverManager.getConnection(jdbcUrl, jdbcUser, jdbcPassword)) {
            try (Statement stmt = conn.createStatement()) {
                String sql = "SELECT SCHEMA, NAME FROM SYS_TABLES";

                try (ResultSet rs = stmt.executeQuery(sql)) {
                    while (rs.next()){
                        String schema = rs.getString("SCHEMA");
                        String name   = rs.getString("NAME");

                        System.out.print("SCHEMA: " + schema);
                        System.out.print(", NAME: " + name);
                        System.out.print("\n");
                    }
                }
            }
        }
    }
}

