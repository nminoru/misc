package jp.nminoru;

import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.parquet.example.data.simple.convert.GroupRecordConverter;
import org.apache.parquet.example.data.Group;
import org.apache.parquet.hadoop.api.ReadSupport;
import org.apache.parquet.io.api.RecordMaterializer;
import org.apache.parquet.schema.MessageType;

public class GroupReadSupport extends ReadSupport<Group> {

    @Override
    public ReadSupport.ReadContext init(Configuration configuration, Map<String, String> keyValueMetaData,
                                        MessageType fileSchema) {
        String partialSchemaString = configuration.get(ReadSupport.PARQUET_READ_SCHEMA);
        MessageType requestedProjection = getSchemaForRead(fileSchema, partialSchemaString);

        return new ReadContext(requestedProjection);
    }

    @Override
    public RecordMaterializer<Group> prepareForRead(Configuration configuration,
                                                    Map<String, String> keyValueMetaData, MessageType fileSchema,
                                                    ReadSupport.ReadContext readContext) {
        return new GroupRecordConverter(readContext.getRequestedSchema());
    }
}
