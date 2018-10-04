package com.example;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;
import java.util.Comparator;    

public class ClientTest {
    public static class TagStruct {
        public List<String> tags;
        
        public TagStruct(List<String> tags) {
            this.tags = tags;
        }
    }
    
    public static void main(String[] args) {
        List<TagStruct> tagStructs = new ArrayList<>();
        
        tagStructs.add(new TagStruct(Arrays.asList(new String[]{"abc", "def", "ghi"})));
        tagStructs.add(new TagStruct(Arrays.asList(new String[]{"abc",        "ghi"})));
        tagStructs.add(new TagStruct(Arrays.asList(new String[]{       "def", "ghi"})));
        tagStructs.add(new TagStruct(Arrays.asList(new String[]{"abc", "def"       })));

        tagStructs.sort(new Comparator<TagStruct>() {
                @Override
                public int compare(TagStruct o1, TagStruct o2) {
                    int len1 = o1.tags.size();
                    int len2 = o2.tags.size();

                    for (int i = 0 ; i < len1 && i < len2 ; i++) {
                        int res;

                        res = o1.tags.get(i).compareTo(o2.tags.get(i));
                        if (res != 0)
                            return res;
                    }

                    return len1 - len2;
                }
            });

        for (TagStruct tagStruct : tagStructs) {
            for (String tag : tagStruct.tags) {
                System.out.print(tag + " ");
            }
            System.out.println();
        }
    }
}
