package io.literal.model;

public enum Motivation {
    TAGGING,
    HIGHLIGHTING;

    public type.Motivation toGraphQL() {
        return type.Motivation.valueOf(this.name());
    }
}
