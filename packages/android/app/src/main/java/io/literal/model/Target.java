package io.literal.model;

import org.json.JSONException;
import org.json.JSONObject;

public class Target {
    public enum Type {
        TEXTUAL_TARGET,
        EXTERNAL_TARGET,
        SPECIFIC_TARGET
    }

    private final Type type;

    public Target(Type type) {
        this.type = type;
    }

    public Type getType() {
        return type;
    }

    public JSONObject toJson() throws JSONException {
       throw new JSONException("Serialization not implemented.");
    }

    public static Target fromJson(JSONObject json) throws JSONException {
        if (json.has("value")) {
            return TextualTarget.fromJson(json);
        } else if (json.has("selector")) {
            return SpecificTarget.fromJson(json);
        } else {
            return ExternalTarget.fromJson(json);
        }
    }
}
