package io.literal.model;

import org.json.JSONException;
import org.json.JSONObject;

public class Body {
    public enum Type {
        TEXTUAL_BODY
    }

    private final Type type;

    public Body(Type type) {
        this.type = type;
    }

    public Type getType() {
        return type;
    }

    public JSONObject toJson() throws JSONException {
        throw new JSONException("toJson not implemented.");
    };

    public static Body fromJson(JSONObject json) throws JSONException {
        String type = json.getString("type");
        if (type.equals(Type.TEXTUAL_BODY.name())) {
            return TextualBody.fromJson(json);
        } else {
            throw new JSONException("fromJson not implemented.");
        }
    }
}
