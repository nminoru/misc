package nminoru;

import java.util.ArrayList;
import java.util.List;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.PropertyException;
import javax.xml.bind.Unmarshaller;

import javax.xml.bind.annotation.XmlElement;

import org.eclipse.persistence.jaxb.JAXBContextFactory;
import org.eclipse.persistence.jaxb.MarshallerProperties;

public class jaxb_test {

    static public class Dog {
        public String name;
        public int age;
        public boolean bitable;
        public List<String> dogs;
        public List<List<String>> multidimarr;

        private String phoneNumbers;

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
        JAXBContext jaxbContext = JAXBContextFactory.createContext(new Class[]{Dog.class}, null);

        Marshaller marshaller = jaxbContext.createMarshaller();
        marshaller.setProperty(MarshallerProperties.MEDIA_TYPE, "application/json");
        marshaller.setProperty(MarshallerProperties.JSON_INCLUDE_ROOT, false);
        marshaller.setProperty(Marshaller.JAXB_FORMATTED_OUTPUT, true);

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

        marshaller.marshal(dog, System.out);
    }
}


