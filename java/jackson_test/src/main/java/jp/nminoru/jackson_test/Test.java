package jp.nminoru.jackson_test;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

public class Test {
    static ObjectMapper mapper = new ObjectMapper();

    public static class Foo {
        public String name;
        public String description;

        public Foo() {
        }

        public Foo(String name, String description) {
            this.name = name;
            this.description = description;
        }        
    }
        
    public static void main(String[] args) throws JsonProcessingException {
        Foo foo = new Foo("NAME", "DESCRIPTION");
        String jsonString = mapper.writeValueAsString(foo);

        System.out.println(jsonString);
    }
}
