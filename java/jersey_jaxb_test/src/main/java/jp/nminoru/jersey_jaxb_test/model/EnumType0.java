package jp.nminoru.jersey_jaxb_test.model;


public enum EnumType0 {
    ABC("abc"),
    XYZ("xyz"),
    UNSPECIFIED("unspecified");

    private final String label;

    private EnumType0(String label) {
        this.label = label;
    }

    public String getLabel() {
        return label;
    }

    public static EnumType0 getEnumType0(String enumType) {
        for (EnumType0 value : EnumType0.values()) {
            if (value.label.equals(enumType))
                return value;
        }

        throw new IllegalArgumentException();
    }
}
