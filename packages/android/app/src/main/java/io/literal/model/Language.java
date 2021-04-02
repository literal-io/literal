package io.literal.model;

public enum Language {
    EN_US;

    public type.Language toGraphQL() {
        return type.Language.valueOf(this.name());
    }
}
