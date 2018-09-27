package com.example;

import java.util.Date;
import java.util.Map;
import com.example.AtlasStruct;

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
