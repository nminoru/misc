package jp.nminoru.atlas_test;

import java.util.List;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasEntitiesWithExtInfo {
    public List<AtlasEntity> entities;
}
