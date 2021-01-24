package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import io.literal.lib.JsonArrayUtil;

public class Annotation {
    private final Body[] body;
    private final Target[] target;
    private final String id;

    public Annotation(Body[] body, @NotNull Target[] target, @NotNull String id) {
        this.body = body;
        this.target = target;
        this.id = id;
    }

    public static Annotation fromJson(JSONObject json) throws JSONException {
        return new Annotation(
                json.has("body")
                        ? JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("body"), new Body[0], Body::fromJson)
                        : null,
                JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("target"), new Target[0], Target::fromJson),
                json.getString("id")
        );
    }

    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("body", this.body != null ? JsonArrayUtil.stringifyObjectArray(this.body, Body::toJson) : null);
        output.put("target", JsonArrayUtil.stringifyObjectArray(this.target, Target::toJson));
        return output;
    }
}
