package com.example;

import java.util.List;
import com.example.AtlasStruct;

public class AtlasEntityHeader extends AtlasStruct {
    public String       guid;
    public String       status;
    public String       displayText;
    public List<String> classificationNames;
    public List<Object> classifications;
    public List<String> meaningNames;
    public List<Object> meanings;
}
