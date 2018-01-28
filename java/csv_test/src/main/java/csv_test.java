import java.io.IOException;
import java.io.FileReader;
import java.io.Reader;
import java.util.Iterator;
import org.apache.commons.csv.CSVFormat;
import org.apache.commons.csv.CSVRecord;

public class csv_test {

    public static void main(String[] args) throws IOException {
        if (args.length < 1)
            return;

        int numColumns = 0;

        try (Reader in = new FileReader(args[0])) {
            Iterable<CSVRecord> records = CSVFormat.EXCEL.parse(in);
            for (CSVRecord record : records) {
                if (numColumns < record.size()) {
                    numColumns = record.size();
                }
            }            
        }

        try (Reader in = new FileReader(args[0])) {
            Iterable<CSVRecord> records = CSVFormat.EXCEL.parse(in);
            for (CSVRecord record : records) {
                System.out.println("*******");
                
                for (Iterator<String> it = record.iterator() ; it.hasNext() ; )
                    System.out.println(it.next());

                System.out.println();
            }            
        }
    }
}