package jp.nminoru.atlas_test;

import java.util.Map;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasEntityExtInfo {
    public Map<String, AtlasEntity> referredEntities;
}
