import java.net.URI;

public class JdbcUrlParse {

    public static void main(String[] args) throws Exception {
        String jdbcUrl = "jdbc:snowflake://nminoru.snowflakecomputing.com/?useProxy=true";

        if (jdbcUrl.startsWith("jdbc:")) {
            URI uri = URI.create(jdbcUrl.substring(5));

            System.out.println("scheme: " + uri.getScheme());
            System.out.println("string: " + uri.toASCIIString());
        }
    }
}
