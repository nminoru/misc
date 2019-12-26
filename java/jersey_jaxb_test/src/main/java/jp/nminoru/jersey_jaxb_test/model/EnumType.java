package jp.nminoru.jersey_jaxb_test.model;

public enum EnumType {
    ABC("abc"),
    XYZ("xyz"),
    UNSPECIFIED("unspecified");

    private final String label;

    private EnumType(String label) {
        this.label = label;
    }

    public String getLabel() {
        return label;
    }

    public static EnumType getEnumType(String enumType) {
        for (EnumType value : EnumType.values()) {
            if (value.label.equals(enumType))
                return value;
        }

        throw new IllegalArgumentException();
    }
}
