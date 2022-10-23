package jp.nminoru.tika_test;

import java.io.InputStreamReader;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.StringWriter;
import org.apache.tika.Tika;
import org.apache.tika.exception.TikaException;
import org.apache.tika.metadata.Metadata;
import org.apache.tika.parser.AutoDetectParser;
import org.apache.tika.parser.ParseContext;
import org.apache.tika.parser.Parser;
import org.apache.tika.sax.BodyContentHandler;
import org.xml.sax.SAXException;

public class Main {
    public static final Tika tika = new Tika();
    public static final Parser parser = new AutoDetectParser();
    
    public static void main(String[] args) throws IOException, SAXException, TikaException {
        for (String fileName : args) {
            parse(fileName);
        }
    }

    static void parse(String fileName) throws IOException, SAXException, TikaException {

        System.out.println("=== File Nmae : " + fileName + " ===");
        
        try (FileInputStream inputStream = new FileInputStream(fileName)) {
            Metadata metadata = new Metadata();
            StringWriter stringWriter = new StringWriter();

            parser.parse(inputStream, new BodyContentHandler(stringWriter), metadata, new ParseContext());

            for (String key : metadata.names()) {
                Object value = metadata.getValues(key);

                if (value instanceof String[]) {
                    String[] array = (String[])value;
                    System.out.println(key + " : " + String.join(",", array));
                } else {
                    System.out.println(key + " : " + value);                    
                }
            }
            // result.text = stringWriter.toString();

            System.out.println();
        }
    }
}
