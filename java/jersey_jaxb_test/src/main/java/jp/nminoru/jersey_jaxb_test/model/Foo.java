package jp.nminoru.jersey_jaxb_test.model;

import java.util.HashMap;
import java.util.Map;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlTransient;
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;
    
import jp.nminoru.jersey_jaxb_test.model.EnumType;
import jp.nminoru.jersey_jaxb_test.model.Options;
import jp.nminoru.jersey_jaxb_test.model.OptionsAdaptor;


public class Foo {

    @XmlTransient
    public EnumType enumType;

    public Integer  version0;
    public Integer  version1;
    public Map<String, Object> options0;

    @XmlJavaTypeAdapter(OptionsAdaptor.class)
    public Options  options1;

    public Foo() {
        this.enumType = EnumType.UNSPECIFIED;
        this.options0 = new HashMap<>();
        this.options1 = new Options();
    }

    @XmlElement(name = "enumType")
    public String getEnumType() {
        if (enumType != EnumType.UNSPECIFIED)
            return enumType.getLabel();
        else
            return null;
    }

    @XmlElement(name = "enumType")
    public void setEnumType(String enumTypeString) {
        if (enumTypeString != null)
            enumType = EnumType.getEnumType(enumTypeString);
        else
            enumType = EnumType.UNSPECIFIED;
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
