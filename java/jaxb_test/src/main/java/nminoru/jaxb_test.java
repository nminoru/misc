package nminoru;

import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.json.Json;
import javax.json.JsonObject;
import javax.json.JsonReader;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.PropertyException;
import javax.xml.bind.Unmarshaller;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;
import javax.xml.bind.annotation.adapters.XmlAdapter;
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;
import javax.xml.transform.stream.StreamSource;
import org.eclipse.persistence.jaxb.JAXBContextFactory;
import org.eclipse.persistence.jaxb.MarshallerProperties;

public class jaxb_test {

    public static class Dog {
        public String name;
        public int age;
        public boolean bitable;
        public List<String> dogs;
        public List<List<String>> multidimarr;
        private String phoneNumbers;
        public Map<String, Object> attributes;
        
        @XmlElement(name="phone-number")
        public String getPhoneNumbers() {
            return phoneNumbers;
        }

        public void setPhoneNumbers(String phoneNumbers) {
            this.phoneNumbers = phoneNumbers;
        }
    }

    public static class Cat {
        public String name;

        @XmlElement(name="options")
        @XmlJavaTypeAdapter(JsonObjectAdapter.class)        
        public JsonObject options;
    }

    public static class JsonObjectAdapter extends XmlAdapter<String, JsonObject> {
        @Override
        public String marshal(JsonObject v) throws Exception {
            if (null == v)
                return null;

            return v.toString();
        }

        @Override
        public JsonObject unmarshal(String v) throws Exception {
            System.out.println("JsonObjectAdapter: " + v);
            
            if (null == v)
                return null;
            
            JsonReader jsonReader = Json.createReader(new StringReader(v));
            return jsonReader.readObject();
        }
    }
    
    public static void main(String[] args) throws JAXBException, PropertyException {
        // JAXBContext jaxbContext = JAXBContext.newInstance(Dog.class);
        Dog dog = new Dog();
        dog.name = "Falco";
        dog.age = 4;
        dog.bitable = false;
        dog.dogs = new ArrayList<String>();
        dog.dogs.add("A");
        dog.dogs.add("B");
        dog.dogs.add("C");

        List<String> array = new ArrayList<>();
        array.add("123");
        array.add("456"); 
        dog.multidimarr = new ArrayList<>();
        dog.multidimarr.add(array);
        dog.multidimarr.add(array);
        dog.multidimarr.add(array);

        dog.setPhoneNumbers("foo");

        String result = marshal(dog);
        System.out.println(result);
        System.out.println();

        dog = unmarshal("{\"name\":\"abc\", \"age\":34}", Dog.class);
        
        System.out.println("name: " + dog.name);
        System.out.println("age: " + dog.age);
        System.out.println();
        
        dog = unmarshal("{\"name\":\"xyz\", \"age\":12, \"bar\":\"baz\"}", Dog.class);

        System.out.println("name: " + dog.name);
        System.out.println("age: " + dog.age);
        System.out.println();

        dog = new Dog();
        dog.attributes = new HashMap<>();
        dog.attributes.put("key1", "value1");
        dog.attributes.put("key2", "value2");
        dog.attributes.put("key3", "value3");
        result = marshal(dog);
        System.out.println(result);        
        System.out.println();        

        JsonReader jsonReader = Json.createReader(new StringReader("{\"prop\":\"value\", \"id\":0, \"path\":\"/\"}"));

        Cat cat = new Cat();
        
        cat.name = "NAME";
        cat.options = jsonReader.readObject();

        result = marshal(cat);
        System.out.println(result);        
        System.out.println();

        cat = unmarshal(result, Cat.class);
        System.out.println("name: " + cat.name);
        System.out.println("options: " + cat.options.toString());
        System.out.println();        
    }
    
    static JAXBContext jaxbContext;

    static {
        try {
            jaxbContext = JAXBContextFactory.createContext(new Class[]{Dog.class, Cat.class}, null);
        } catch (JAXBException e) {
        }
    }

    static String marshal(Object object) throws JAXBException {
        Marshaller marshaller = jaxbContext.createMarshaller();
        
        marshaller.setProperty(MarshallerProperties.MEDIA_TYPE, "application/json");
        marshaller.setProperty(MarshallerProperties.JSON_INCLUDE_ROOT, false);
        marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, true);

        StringWriter marshllerOutput = new StringWriter();
        
        marshaller.marshal(object, marshllerOutput);

        return marshllerOutput.toString();
    }
    
    static <T> T unmarshal(String jsonString, Class<T> type) throws JAXBException {
        Unmarshaller unmarshaller = jaxbContext.createUnmarshaller();
        
        unmarshaller.setProperty(MarshallerProperties.MEDIA_TYPE, "application/json");
        unmarshaller.setProperty(MarshallerProperties.JSON_INCLUDE_ROOT, false);
        
        StringReader reader = new StringReader(jsonString);
        JAXBElement<T> element = unmarshaller.unmarshal(new StreamSource(reader), type);

        return element.getValue();
    }
}


