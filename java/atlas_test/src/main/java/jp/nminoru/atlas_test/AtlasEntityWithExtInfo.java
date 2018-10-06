package jp.nminoru.atlas_test;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasEntityWithExtInfo extends AtlasEntityExtInfo {
    public AtlasEntity entity;

    public AtlasEntityWithExtInfo() {
        super();
        entity = new AtlasEntity();
    }
}
