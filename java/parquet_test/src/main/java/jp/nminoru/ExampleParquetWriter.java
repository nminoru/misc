package jp.nminoru;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.parquet.column.ParquetProperties;
import org.apache.parquet.example.data.Group;
import org.apache.parquet.hadoop.ParquetWriter;
import org.apache.parquet.hadoop.api.WriteSupport;
import org.apache.parquet.hadoop.metadata.CompressionCodecName;
import org.apache.parquet.schema.MessageType;

/**
 * An example file writer class.
 * THIS IS AN EXAMPLE ONLY AND NOT INTENDED FOR USE.
 */
public class ExampleParquetWriter extends ParquetWriter<Group> {

    /**
     * Creates a Builder for configuring ParquetWriter with the example object
     * model. THIS IS AN EXAMPLE ONLY AND NOT INTENDED FOR USE.
     *
     * @param file the output file to create
     * @return a {@link Builder} to create a {@link ParquetWriter}
     */
    public static Builder builder(Path file) {
        return new Builder(file);
    }

    /**
     * Create a new {@link ExampleParquetWriter}.
     *
     * @param file The file name to write to.
     * @param writeSupport The schema to write with.
     * @param compressionCodecName Compression code to use, or CompressionCodecName.UNCOMPRESSED
     * @param blockSize the block size threshold.
     * @param pageSize See parquet write up. Blocks are subdivided into pages for alignment and other purposes.
     * @param enableDictionary Whether to use a dictionary to compress columns.
     * @param conf The Configuration to use.
     * @throws IOException
     */
    ExampleParquetWriter(Path file, WriteSupport<Group> writeSupport,
                         CompressionCodecName compressionCodecName,
                         int blockSize, int pageSize, boolean enableDictionary,
                         boolean enableValidation,
                         ParquetProperties.WriterVersion writerVersion,
                         Configuration conf) throws IOException {
        super(file, writeSupport, compressionCodecName, blockSize, pageSize,
              pageSize, enableDictionary, enableValidation, writerVersion, conf);
    }

    public static class Builder extends ParquetWriter.Builder<Group, Builder> {
        private MessageType type = null;
        private Map<String, String> extraMetaData = new HashMap<String, String>();
        
        private Builder(Path file) {
            super(file);
        }

        public Builder withType(MessageType type) {
            this.type = type;
            return this;
        }

        public Builder withExtraMetaData(Map<String, String> extraMetaData) {
            this.extraMetaData = extraMetaData;
            return this;
        }

        @Override
        protected Builder self() {
            return this;
        }

        @Override
        protected WriteSupport<Group> getWriteSupport(Configuration conf) {
            return new GroupWriteSupport(type, extraMetaData);
        }
    }
}
