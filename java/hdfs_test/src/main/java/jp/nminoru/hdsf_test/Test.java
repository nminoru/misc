package jp.nminoru.hdfs_test;

import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.permission.FsPermission;
import org.apache.hadoop.security.UserGroupInformation;


public class Test {
    public static void main(String[] agrs) throws IOException {
        Configuration conf = new Configuration();

        conf.set("fs.defaultFS", "hdfs://127.0.0.1:9000");
        // conf.set("hadoop.job.ugi", "nminoru,supergroup");
        // conf.set("hadoop.job.ugi", "nobody,supergroup");

        try (FileSystem fileSystem = FileSystem.get(conf)) {
            for (FileStatus fileStatus : fileSystem.listStatus(new Path("/user")))
                System.out.println(fileStatus.getPath());
        }
    }
}

