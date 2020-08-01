package jp.nminoru.file_copy_test;

import java.io.File;
import java.io.IOException;
import java.nio.file.FileVisitOption;
import java.nio.file.FileVisitResult;
import java.nio.file.FileVisitor;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.EnumSet;
import java.util.HashSet;
import java.util.Set;
import org.apache.commons.io.FileUtils;

public class CopyFileVisitor implements FileVisitor<Path> {
    Path srcRootPath;
    Path destRootPath;

    public CopyFileVisitor(Path srcPath, Path destPath) {
        super();
        srcRootPath = srcPath;
        destRootPath = destPath;
    }

    /**
     * ディレクトリ内のエントリがビジットされる前に、そのディレクトリに対して呼び出される。
     */
    @Override
    public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
        Path p = destRootPath.resolve(srcRootPath.relativize(dir));
        System.out.println("preVisitDirectory: " + dir.toString() + " -> " + p.toString());

        File dirFile = dir.toFile();

        if (destFile.exists()) {
            if (destFile.isDirectory()) {
                FileUtils.forceDelete(destFile);
            }
        } else {
            FileUtils.forceMkdir(destFile);
        }
        
        return FileVisitResult.CONTINUE;
    }

    /**
     * ディレクトリ内のエントリ、およびそのすべての子孫がビジットされたあとにそのディレクトリに対して呼び出されます。
     * このメソッドは、ディレクトリの反復処理が早く完了しすぎた場合(visitFileメソッドがSKIP_SIBLINGSを返したり、ディレクトリに対する反復処理時に入出力エラーが発生したりすることにより)にも呼び出されます。
     */
    @Override
    public FileVisitResult postVisitDirectory(Path dir, IOException exc) throws IOException {
        return FileVisitResult.CONTINUE;        
    }    

    /**
     * ディレクトリ内のファイルに対して呼び出されます。
     */
    @Override
    public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
        Path p = destRootPath.resolve(srcRootPath.relativize(file));
        System.out.println("visitFile: " + file.toString() + " -> " + p.toString());
        return FileVisitResult.CONTINUE;        
    }

    /**
     * ビジットできなかったファイルに対して呼び出されます。このメソッドは、ファイルの属性を読み取れなかったり、ファイルが開けないディレクトリだったりなどの理由の場合に呼び出されます。
     */
    @Override
    public FileVisitResult visitFileFailed(Path file, IOException exc) throws IOException {
        return FileVisitResult.CONTINUE;
    }

    static void copyFiles(Path srcPath, Path destPath) throws IOException {
        CopyFileVisitor visitor = new CopyFileVisitor(srcPath, destPath);

        // final Set<FileVisitOption> options = EnumSet.of(FileVisitOption.FOLLOW_LINKS);
        final Set<FileVisitOption> options = new HashSet<>();
        final int maxDepth = Integer.MAX_VALUE;

        File srcFile = srcPath.toFile();

        if (srcFile.isFile())
            copyFile(srcPath, destPath);
        else if (srcFile.isDirectory()) {
            Files.walkFileTree(srcPath, options, maxDepth, visitor);
        }
    }

    static void copyFile(Path srcPath, Path destPath) throws IOException {
        File srcFile = srcPath.toFile();
        File destFile = destPath.toFile();
        
        if (destFile.exists() && !destFile.isFile())
            FileUtils.forceDelete(destFile);
        
        FileUtils.copyFile(srcFile, destFile);
    }
}
