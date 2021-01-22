package io.literal.model;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.lib.JsonArrayUtil;

public class TextPositionSelector extends Selector {
    private final int start;
    private final int end;
    private final Selector[] refinedBy;

    public TextPositionSelector(int start, int end) {
        super(Type.TEXT_POSITION_SELECTOR);
        this.start = start;
        this.end = end;
        this.refinedBy = null;
    }

    public TextPositionSelector(int start, int end, Selector[] refinedBy) {
        super(Type.TEXT_POSITION_SELECTOR);
        this.start = start;
        this.end = end;
        this.refinedBy = refinedBy;
    }

    public static TextPositionSelector fromJson(JSONObject json) throws JSONException {
        return new TextPositionSelector(
                json.getInt("start"),
                json.getInt("end"),
                json.has("refinedBy")
                        ? JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("refinedBy"), new Selector[0], Selector::fromJson)
                        : null
        );
    }

    public int getStart() {
        return start;
    }

    public int getEnd() {
        return end;
    }

    public Selector[] getRefinedBy() {
        return refinedBy;
    }

    @Override
    public JSONObject toJson() throws JSONException {
        JSONObject result = new JSONObject();

        result.put("type", getType());
        result.put("start", getStart());
        result.put("end", getEnd());
        result.put("refinedBy",
                this.refinedBy != null ? JsonArrayUtil.stringifyObjectArray(this.refinedBy, Selector::toJson) : null);

        return result;
    }
}
