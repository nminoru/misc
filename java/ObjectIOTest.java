import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public class ObjectIOTest {
    
    public static void main(String[] args) throws Exception {
        File file = File.createTempFile("temp", null);
        file.deleteOnExit();

        try (ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(file))) {
            out.writeObject("abc");
            out.writeObject("vw\nxyz");            
        }
        
        try (ObjectInputStream in = new ObjectInputStream(new FileInputStream(file))) {
            while (true) {
                String text = (String)in.readObject();
                System.out.println("object: " + text);
            }
        } catch (EOFException e) {
            System.out.println("end");
        }
    }
}
