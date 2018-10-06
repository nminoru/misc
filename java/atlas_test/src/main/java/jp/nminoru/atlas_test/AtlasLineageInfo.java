package jp.nminoru.atlas_test;

import java.util.List;
import java.util.Map;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasLineageInfo {
    public static class LineageRelation {
        public String fromEntityId;
        public String toEntityId;
        public String relationshipId;
    }
    
    public String baseEntityGuid;
    public String lineageDirection;
    public int    lineageDepth;
    public Map<String, AtlasEntityHeader> guidEntityMap;
    public List<LineageRelation>          relations;
}
