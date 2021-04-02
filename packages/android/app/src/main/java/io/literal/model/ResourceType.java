package io.literal.model;

public enum ResourceType {
    DATASET,
    IMAGE,
    VIDEO,
    SOUND,
    TEXT;

    public type.ResourceType toGraphQL() {
        return type.ResourceType.valueOf(this.name());
    }
}