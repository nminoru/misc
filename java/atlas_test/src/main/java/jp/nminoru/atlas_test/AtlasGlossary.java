package jp.nminoru.atlas_test;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasGlossary {
    public String guid;
    public String qualifiedName;
    public String name;
    public String shortDescription;
    public String longDescription;
}
