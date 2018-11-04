package jp.nminoru.file_copy_test;

import java.io.IOException;
import java.nio.file.Path;
import java.nio.file.Paths;

import jp.nminoru.file_copy_test.CopyFileVisitor;

public class Main {
    public static void main(String[] args) throws IOException {
        CopyFileVisitor.copyFiles(Paths.get(args[0]), Paths.get(args[1]));
    }
}
