package io.literal.model;

public enum Format {
    TEXT_PLAIN,
    TEXT_HTML;

    public type.Format toGraphQL() {
        return type.Format.valueOf(this.name());
    }
}
