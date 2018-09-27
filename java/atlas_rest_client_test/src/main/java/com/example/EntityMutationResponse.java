package com.example;

import java.util.List;
import java.util.Map;

import com.example.AtlasEntityHeader;

public class EntityMutationResponse {
    public Map<String, List<AtlasEntityHeader>> mutatedEntities;
    public Map<String, String>                  guidAssignments;    
}
