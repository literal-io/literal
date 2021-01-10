package io.literal.model;

import android.util.JsonReader;

import org.json.JSONException;
import org.json.JSONObject;

public abstract class Selector {
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

    public abstract JSONObject toJson() throws JSONException;
}
