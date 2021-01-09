package io.literal.model;

public class Selector {
    public enum Type {
        RANGE_SELECTOR,
        XPATH_SELECTOR,
        TEXT_POSITION_SELECTOR
    };

    private final Type type;

    public Selector(Type type) {
        this.type = type;
    }

    public Type getType() {
        return type;
    }
}
