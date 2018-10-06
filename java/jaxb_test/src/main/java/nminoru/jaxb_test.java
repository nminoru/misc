package nminoru;

import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.PropertyException;
import javax.xml.bind.Unmarshaller;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElement;
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

        marshal(dog);
        
        System.out.println();

        dog = unmarshal("{\"name\":\"abc\", \"age\":34}");
        
        System.out.println("name: " + dog.name);
        System.out.println("age: " + dog.age);
        System.out.println();
        
        dog = unmarshal("{\"name\":\"xyz\", \"age\":12, \"bar\":\"baz\"}");

        System.out.println("name: " + dog.name);
        System.out.println("age: " + dog.age);
        System.out.println();

        dog = new Dog();
        dog.attributes = new HashMap<>();
        dog.attributes.put("key1", "value1");
        dog.attributes.put("key2", "value2");
        dog.attributes.put("key3", "value3");
        marshal(dog);
        System.out.println();
    }
    
    static JAXBContext jaxbContext;

    static {
        try {
            jaxbContext = JAXBContextFactory.createContext(new Class[]{Dog.class}, null);
        } catch (JAXBException e) {
        }
    }

    static void marshal(Object object) throws JAXBException {
        Marshaller marshaller = jaxbContext.createMarshaller();
        
        marshaller.setProperty(MarshallerProperties.MEDIA_TYPE, "application/json");
        marshaller.setProperty(MarshallerProperties.JSON_INCLUDE_ROOT, false);
        marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, true);
        
        marshaller.marshal(object, System.out);
    }
    
    static Dog unmarshal(String jsonString) throws JAXBException {
        Unmarshaller unmarshaller = jaxbContext.createUnmarshaller();
        
        unmarshaller.setProperty(MarshallerProperties.MEDIA_TYPE, "application/json");
        unmarshaller.setProperty(MarshallerProperties.JSON_INCLUDE_ROOT, false);

        // StringReader reader = new StringReader("{\"name\":\"abc\", \"age\":34}");
        StringReader reader = new StringReader(jsonString);
        JAXBElement<Dog> element = unmarshaller.unmarshal(new StreamSource(reader), Dog.class);

        return element.getValue();
    }
}


