package jp.nminoru.atlas_test;

import java.util.List;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasTypesDef {
    // public List<AtlasEnumDef>           enumDefs;
    public List<AtlasStructDef>         structDefs;
    // public List<AtlasClassificationDef> classificationDefs;
    public List<AtlasEntityDef>         entityDefs;
    // public List<AtlasRelationshipDef>   relationshipDefs;    
}
