import javax.json.bind.JsonbBuilder;
import javax.json.bind.Jsonb;

import org.junit.Assert;
import org.junit.Test;

class Dog {
    public String name;
    public int age;
    public boolean bitable;
}

public class jsonb_test {

    @Test
    public void testRun() {
        System.out.println("*** jsonb_test.testRun() ***");

        // Create a dog instance
        Dog dog = new Dog();
        dog.name = "Falco";
        dog.age = 4;
        dog.bitable = false;

        // Create Jsonb and serialize
        Jsonb jsonb = JsonbBuilder.create();
        String result = jsonb.toJson(dog);

        Assert.assertNotNull(result);

        System.out.println(result);

        // Deserialize back
        dog = jsonb.fromJson("{\"name\":\"Falco\",\"age\":4,\"bitable\":false}", Dog.class);
    }
} 