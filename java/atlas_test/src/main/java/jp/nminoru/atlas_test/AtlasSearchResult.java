package jp.nminoru.atlas_test;

import java.util.List;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

@JsonIgnoreProperties(ignoreUnknown=true)
public class AtlasSearchResult {
    public String                         queryType;
    // public SearchParameters               searchParameters;
    public String                         queryText;
    public String                         type;
    // public String                         classification;
    public List<AtlasEntityHeader>        entities;
    // public AttributeSearchResult          attributes;
    // public List<AtlasFullTextResult>      fullTextResult;
    // public Map<String, AtlasEntityHeader> referredEntities;
}
