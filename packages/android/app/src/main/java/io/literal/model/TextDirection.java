package io.literal.model;

public enum TextDirection {
    LTR,
    RTL,
    AUTO;

    public type.TextDirection toGraphQL() {
        return type.TextDirection.valueOf(this.name());
    }
}
