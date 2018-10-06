package jp.nminoru.atlas_test;

import java.util.Date;
import java.util.Map;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasEntity extends AtlasStruct {
    public String guid;
    public String status;
    public String createdBy;
    public String updatedBy;
    public Date   createTime;
    public Date   updateTime;
    public Long   version = 0L;
    public Map<String, Object>             relationshipAttributes;
    // private List<AtlasClassification>       classifications;
    // private List<AtlasTermAssignmentHeader> meanings;    
}
