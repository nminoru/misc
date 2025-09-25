package jp.nminoru.jersey_jaxb_test.model;

import java.util.HashMap;
import java.util.Map;
import jakarta.xml.bind.annotation.XmlElement;
import jakarta.xml.bind.annotation.XmlTransient;
import jakarta.xml.bind.annotation.adapters.XmlJavaTypeAdapter;

import jp.nminoru.jersey_jaxb_test.model.EnumType0;
import jp.nminoru.jersey_jaxb_test.model.EnumType1;
import jp.nminoru.jersey_jaxb_test.model.MyHashMap;
import jp.nminoru.jersey_jaxb_test.model.Options;
import jp.nminoru.jersey_jaxb_test.model.OptionsAdaptor;


public class Foo {

    @XmlTransient
    public EnumType0 enumType0;

    public EnumType1 enumType1;

    private EnumType1 _enumType2;    

    public int      int0;
    public Integer  int1;

    public Integer  version0;
    public Integer  version1;
    public Map<String, Object> options0;

    @XmlTransient
    public Options  options1;

    public MyHashMap options2;

    @XmlTransient
    public Integer  transperent0 = Integer.valueOf(1);

    @XmlTransient
    public Integer  transperent1 = Integer.valueOf(1);

    public Foo() {
        this.enumType0 = EnumType0.UNSPECIFIED;
        this.enumType1 = EnumType1.unspecified;
        this.int0      = 1;
        this.int1      = Integer.valueOf(1);
        this.options0  = new HashMap<>();
        this.options1  = new Options();
        this.options2  = new MyHashMap();
    }

    @XmlElement(name = "enumType0")
    public String getEnumType0() {
        if (enumType0 != null)
            return enumType0.getLabel();
        else
            return EnumType0.UNSPECIFIED.getLabel();
    }

    @XmlElement(name = "enumType0")
    public void setEnumType0(String enumTypeString) {
        if (enumTypeString != null)
            enumType0 = EnumType0.getEnumType0(enumTypeString);
        else
            enumType0 = EnumType0.UNSPECIFIED;
    }

    @XmlElement(name = "enumType2")
    public String getEnumTypeX() {
        return EnumType1.unspecified.name();
    }

    @XmlElement(name = "enumType2")
    public void setEnumTypeX(String enumTypeString) {
        if (enumTypeString != null)
            _enumType2 = EnumType1.valueOf(enumTypeString);
        else
            _enumType2 = EnumType1.unspecified;
    }    

    /**
     * version0 に重なる getter があった場合、@XmlTransient を指定しても
     * アンマーシャリングできなくなる。
     *
     * version0 = null となる。
     */
    @XmlTransient
    public int getVersion0() {
        return version0 != null ? version0.intValue() : 1;
    }

    @XmlElement(name = "options1")
    public Map<String, Object> getOptions1() {
        System.out.println("*** Foo.getOptions1 ***: " + options1._key1);

        Map<String, Object> map = new HashMap<>();

        map.put("key1", options1._key1);

        return map;
    }

    @XmlElement(name = "options1")
    public void setOptions1(Map<String, Object> map) {
        System.out.println("*** Foo.setOptions1 ***: " + map.get("key1"));

        options1 = new Options();
        options1._key1 = map.get("key1");
    }    

    @XmlElement(name = "transperent1")
    public Integer getTransparentY() {
        System.out.println("*** Foo.getTransparent1 ***");
        return transperent1;
    }

    @XmlElement(name = "transperent1")
    public void setTransparentY(int value) {
        System.out.println("*** Foo.setTransparent1 ***: " + value);
        
        transperent1 = value + 1;
    }
}
