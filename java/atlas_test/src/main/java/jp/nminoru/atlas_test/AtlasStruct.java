package jp.nminoru.atlas_test;

import java.util.HashMap;
import java.util.Map;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasStruct {
    public String              typeName;
    public Map<String, Object> attributes;

    public AtlasStruct() {
        attributes = new HashMap<>();
    }
}
