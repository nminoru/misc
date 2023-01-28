package jp.nminoru.file_lines_test;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;


/**
 * Files.lines(path) によって開かれる Steam をリークさせた場合に GC 時に回収されるか確認する
 */
public class Test {

    public static void main(String[] args) throws InterruptedException, IOException {
        String dataString = null;
        Path path = Paths.get("aaa.txt");

        while (true) {
            dataString = Files.lines(path).reduce("", (prev, line) -> prev + line);
            System.gc();            
            Thread.sleep(1000L);
        }
    }
}
