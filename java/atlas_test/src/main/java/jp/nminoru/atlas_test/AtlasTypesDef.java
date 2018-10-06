package jp.nminoru.atlas_test;

import java.util.List;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasTypesDef {
    // public List<AtlasEnumDef>           enumDefs;
    public List<AtlasStructDef>         structDefs;
    // public List<AtlasClassificationDef> classificationDefs;
    public List<AtlasEntityDef>         entityDefs;
    // public List<AtlasRelationshipDef>   relationshipDefs;    
}
