import java.nio.file.Files;
import java.nio.file.Path;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

public class ZipCompress {
    
    public void createZip(String zipFilename, String[] inputFilenames) throws IOException {
        ZipOutputStream zipfile = null;

        try {
            zipfile = new ZipOutputStream(new FileOutputStream(zipFilename));

            for (String fileName : inputFilenames) {
                File file = new File(fileName);

                addFile(zipfile, "", file);
            }
        } finally {
            zipfile.close();
        }
    }

    void addFile(ZipOutputStream zipFile, String base, File file) throws IOException {
        String fileName = file.getName();

        if (!file.exists())
            return;

        String filePath = base.equals("") ? fileName : base + File.separator + fileName;

        if (file.isFile()) {
            zipFile.putNextEntry(new ZipEntry(filePath));
            
            Files.copy(file.toPath(), zipFile);

            zipFile.closeEntry();

        } else if (file.isDirectory()) {
            if (fileName.equals(".") || fileName.equals(".."))
                return;

            for (File f : file.listFiles()) {
                addFile(zipFile, filePath, f);
            }
        }
    }

    public static void main(String[] args) {
        ZipCompress zipCompress = new ZipCompress();

        try {
            zipCompress.createZip(args[0], Arrays.copyOfRange(args, 1, args.length));
        } catch (IOException e) {
            e.printStackTrace();
        } 
    }
}
