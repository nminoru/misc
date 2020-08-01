package jp.nminoru.jersey_jaxb_test.model;

import java.util.HashMap;
import java.util.Map;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlTransient;
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;

import jp.nminoru.jersey_jaxb_test.model.EnumType0;
import jp.nminoru.jersey_jaxb_test.model.EnumType1;
import jp.nminoru.jersey_jaxb_test.model.Options;
import jp.nminoru.jersey_jaxb_test.model.OptionsAdaptor;


public class Foo {
    
    @XmlTransient
    public EnumType0 enumType0;

    public EnumType1 enumType1;

    public int      int0;
    public Integer  int1;

    public Integer  version0;
    public Integer  version1;
    public Map<String, Object> options0;

    @XmlJavaTypeAdapter(OptionsAdaptor.class)
    public Options  options1;

    public Foo() {
        this.enumType0 = EnumType0.UNSPECIFIED;
        this.enumType1 = EnumType1.unspecified;
        this.int0      = 1;
        this.int1      = new Integer(1);
        this.options0  = new HashMap<>();
        this.options1  = new Options();
    }

    @XmlElement(name = "enumType0")
    public String getEnumType0() {
        if (enumType0 != null && enumType0 != EnumType0.UNSPECIFIED)
            return enumType0.getLabel();
        else
            return null;
    }

    @XmlElement(name = "enumType0")
    public void setEnumType0(String enumTypeString) {
        if (enumTypeString != null)
            enumType0 = EnumType0.getEnumType0(enumTypeString);
        else
            enumType0 = EnumType0.UNSPECIFIED;
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
}
