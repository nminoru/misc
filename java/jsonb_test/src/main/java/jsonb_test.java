import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import javax.json.bind.JsonbBuilder;
import javax.json.bind.Jsonb;

public class jsonb_test {
    Jsonb jsonb = JsonbBuilder.create();    

    public static class Dog {
        public String name;
        public int age;
        public boolean bitable;
        public List<List<String>> multidimarr;
    }

    public static class Bar {
        public String name;

        public Bar() {
        }

        public Bar(String name) {
            this.name = name;
        }
    }

    public static class Baz {
        public String name;

        public Baz() {
        }

        public Baz(String name) {
            this.name = name;
        }        
    }    

    public static class Foo {
        public List<Bar> bars;
        public List<Baz> bazs;
    }    

    public static void main(String[] args) {
        jsonb_test test = new jsonb_test();

        test.testPojo1();
        test.testPojo2();
        test.testPojo3();
        test.testPojo4();
        test.testPojo5();
    }

    /**
     * 配列の配列を含む Java オブジェクトを JSON 文字列へマーシャリング
     */    
    void testPojo1() {
        Dog dog = new Dog();
        dog.name = "Falco";
        dog.age = 4;
        dog.bitable = false;

        List<String> array = new ArrayList<>();
        array.add("123");
        array.add("456"); 
        dog.multidimarr = new ArrayList<>();
        dog.multidimarr.add(array);
        dog.multidimarr.add(array);
        dog.multidimarr.add(array);

        String result = jsonb.toJson(dog);

        System.out.println("test1: " + result);

        dog = jsonb.fromJson(result, Dog.class);
    }

    /**
     * 定義していない "abc" というフィールドを含む JSON 文字列を Java クラスへアンマーシャリング
     */
    void testPojo2() {
        Dog dog = jsonb.fromJson("{\"name\":\"hoge\", \"abc\":\"hoge\"}", Dog.class);
        String result = jsonb.toJson(dog);
        
        System.out.println("test2: " + result);
    }

    /**
     * 配列からはじまる JSON 文字列を Java クラスへアンマーシャリング
     */
    void testPojo3() {
        List<Dog> dogList =
            jsonb.fromJson("[{\"name\":\"foo\", \"abc\":\"ABC\"}, {\"name\":\"baz\", \"abc\":\"XYZ\"}]",
                           new ArrayList<Dog>(){}.getClass().getGenericSuperclass());

        String result = jsonb.toJson(dogList);
        System.out.println("test3: " + result);
    }

    /**
     * 配列からはじまる JSON 文字列を Java クラスへアンマーシャリング
     */
    void testPojo4() {
        Map<String, Object> jsonMap =
            jsonb.fromJson("{\"abc\":\"ABC\", \"def\":\"DEF\", \"ghi\":{\"F1\":1, \"F2\":2, \"F3\":3}}",
                           new HashMap<String, Object>(){}.getClass().getGenericSuperclass());

        printMap(0, jsonMap);
    }

    void testPojo5() {
        Foo foo = new Foo();

        foo.bars = new ArrayList<>();
        foo.bazs = new ArrayList<>();

        foo.bars.add(new Bar("1"));
        foo.bars.add(new Bar("2"));
        foo.bars.add(new Bar("3"));

        foo.bazs.add(new Baz("A"));
        foo.bazs.add(new Baz("B"));
        foo.bazs.add(new Baz("C"));

        String result = jsonb.toJson(foo);

        System.out.println("test5: " + result);
    }    

    void printMap(int level, Map<String, Object> jsonMap) {
        for (String key : jsonMap.keySet()) {
            Object value = jsonMap.get(key);
            
            if (value instanceof Map) {
                for (int i = 0 ; i < level ; i++)
                    System.out.print("    ");
                
                System.out.println(key + ": {");
                
                printMap(level + 1, (Map<String, Object>)value);

                for (int i = 0 ; i < level ; i++)
                    System.out.print("    ");                
                System.out.println("}");
            } else {
                for (int i = 0 ; i < level ; i++)
                    System.out.print("    ");
                
                System.out.println(key + ": " + value.toString());
            }
        }        
    }
}


