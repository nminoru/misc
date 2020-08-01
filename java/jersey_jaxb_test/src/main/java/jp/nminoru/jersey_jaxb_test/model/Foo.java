package jp.nminoru.jersey_jaxb_test.model;

import java.util.HashMap;
import java.util.Map;
import javax.json.bind.Jsonb;
import javax.json.bind.JsonbBuilder;
import javax.json.bind.JsonbException;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlTransient;
import jp.nminoru.jersey_jaxb_test.model.EnumType;


public class Foo {
    static final Jsonb Jsonb = JsonbBuilder.create();

    @XmlTransient
    public EnumType enumType;

    public Integer  version0;
    public Integer  version1;
    public Map<String, Object> options0;

    public Foo() {
        this.enumType = EnumType.UNSPECIFIED;
        this.options0 = new HashMap<>();
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
