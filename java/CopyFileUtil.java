import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.FileVisitor;
import java.nio.file.FileVisitResult;
import java.nio.file.LinkOption;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.SimpleFileVisitor;
import java.nio.file.StandardCopyOption;
import java.nio.file.attribute.BasicFileAttributes;

    
public class CopyFileUtil {

    public static void main(String[] args) throws IOException {
        copyFolder(Paths.get(args[0]), Paths.get(args[1]));
    }
    
    public static void copyFolder(Path source, Path destination) throws IOException {
        
		FileVisitor<Path> visitor = new SimpleFileVisitor<Path>() {
                
			@Override
			public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
				//ディレクトリをコピーする
                copy(dir);
                
				return FileVisitResult.CONTINUE;
            }
			
			@Override
			public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
				//ファイルをコピーする
                copy(file);
                     
				return FileVisitResult.CONTINUE;
            }

            void copy(Path file) throws IOException {
                Path target = destination.resolve(source.relativize(file));

                Files.copy(file, target, StandardCopyOption.REPLACE_EXISTING, StandardCopyOption.COPY_ATTRIBUTES, LinkOption.NOFOLLOW_LINKS);
            }
        };
		
		//ファイルツリーを辿ってFileVisitorの動作をさせる
        Files.walkFileTree(source, visitor);
    }
}
