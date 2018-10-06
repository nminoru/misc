package jp.nminoru.atlas_test;

import java.util.List;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasEntityHeader extends AtlasStruct {
    public String       guid;
    public String       status;
    public String       displayText;
    public List<String> classificationNames;
    public List<Object> classifications;
    public List<String> meaningNames;
    public List<Object> meanings;
}
