import javax.json.bind.JsonbBuilder;
import javax.json.bind.Jsonb;

public class jsonb_test {

    static public class Dog {
        public String name;
        public int age;
        public boolean bitable;
    }

    public static void main(String[] args) {
        // Create a dog instance
        Dog dog = new Dog();
        dog.name = "Falco";
        dog.age = 4;
        dog.bitable = false;

        // Create Jsonb and serialize
        Jsonb jsonb = JsonbBuilder.create();

        String result = jsonb.toJson(dog);

        System.out.println(result);

        // Deserialize back
        dog = jsonb.fromJson("{\"name\":\"Falco\",\"age\":4,\"bitable\":false}", Dog.class);
    }
}


