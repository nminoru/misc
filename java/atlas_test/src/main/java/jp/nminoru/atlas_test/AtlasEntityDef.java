package jp.nminoru.atlas_test;

import java.util.List;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasEntityDef extends AtlasStructDef {
    public List<String> superTypes;
    public List<String> subTypes;
}
