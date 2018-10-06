package jp.nminoru.atlas_test;

import java.util.List;
import java.util.Map;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasStructDef extends AtlasBaseTypeDef {
    
    public static class AtlasAttributeDef {
        public String                   name;
        public String                   typeName;
        public boolean                  isOptional;
        public String                   cardinality;
        public int                      valuesMinCount;
        public int                      valuesMaxCount;
        public boolean                  isUnique;
        public boolean                  isIndexable;
        public boolean                  includeInNotification;
        public String                   defaultValue;
        public String                   description;

        public List<AtlasConstraintDef> constraints;        
    }

    public static class AtlasConstraintDef {
        public String              type;
        public Map<String, Object> params;
    }    
    
    public List<AtlasAttributeDef> attributeDefs;
}
