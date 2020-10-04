import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.List;
import java.util.Objects;
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

    public static class Cat {
        public String name;
        public Map<String, Object> obj;
    }

    public static class Foo {
        public List<Bar> bars;
        public List<Baz> bazs;
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
    
    public static class Qux extends HashMap<String, Object> {
        public String key1;
    }

    public static void main(String[] args) {
        jsonb_test test = new jsonb_test();

        test.testPojo1();
        test.testPojo2();
        test.testPojo3();
        test.testPojo4();
        test.testPojo5();
        test.testPojo6();
        test.testPojo7();        
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

        dog = jsonb.fromJson((String)null, Dog.class);
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

    void testPojo6() {
        String request = "{\"name\":\"NAME\",\"obj\":{\"abc\":\"ABC\", \"def\":\"DEF\", \"ghi\":{\"F1\":1, \"F2\":2, \"F3\":3},\"array\":[0,1,2]}}";
        Cat    cat     = jsonb.fromJson(request, Cat.class);
        String result  = jsonb.toJson(cat);

        System.out.println("test6: " + request);
        System.out.println("test6: " + result);
    }

    void testPojo7() {
        String request = "{\"key1\":\"value1\",\"key2\":\"value2\"}";
        Qux    qux     = jsonb.fromJson(request, Qux.class);
        String result  = jsonb.toJson(qux);

        System.out.println("test7: " + request);
        System.out.println("test7: " + result);
        System.out.println("test7: " + qux.key1); //

        //
        Qux    qux2     = new Qux();
        qux2.key1       = "value1";
        result          = jsonb.toJson(qux2);
        System.out.println("test7: " + result);

        Qux    qux3     = new Qux();
        qux3.key1       = "value1";
        qux3.put("key1", "value1");
        result          = jsonb.toJson(qux3);
        System.out.println("test7: " + result);
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


