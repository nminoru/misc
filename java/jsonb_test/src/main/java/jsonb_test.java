import java.util.ArrayList;
import java.util.List;
import javax.json.bind.JsonbBuilder;
import javax.json.bind.Jsonb;

public class jsonb_test {

    public static class Dog {
        public String name;
        public int age;
        public boolean bitable;
        public List<List<String>> multidimarr;
    }

    public static void main(String[] args) {
        jsonb_test test = new jsonb_test();

        test.testPojo1();
        test.testPojo2();
    }

    void testPojo1() {
        // Create a dog instance
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

        // Create Jsonb and serialize
        Jsonb jsonb = JsonbBuilder.create();

        String result = jsonb.toJson(dog);

        System.out.println(result);

        // Deserialize back
        dog = jsonb.fromJson(result, Dog.class);
    }

    void testPojo2() {
        // Create Jsonb and serialize
        Jsonb jsonb = JsonbBuilder.create();

        // 定義していない "abc" というフィールドをデシリアライズ
        Dog dog = jsonb.fromJson("{\"name\":\"hoge\", \"abc\":\"hoge\"}", Dog.class);
        String result = jsonb.toJson(dog);
        System.out.println(result);
    }    
}


