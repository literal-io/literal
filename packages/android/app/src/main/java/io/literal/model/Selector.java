package io.literal.model;

import android.util.JsonReader;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.repository.ErrorRepository;
import type.AnnotationTargetInput;
import type.SelectorInput;

public class Selector {
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

    public JSONObject toJson() throws JSONException {
        throw new JSONException("toJson not implemented.");
    };

    public SelectorInput toSelectorInput() {
        ErrorRepository.captureException(new Exception("Attempted to call toSelectorInput, but not implemented."));
        return null;
    }

    public static Selector fromJson(JSONObject json) throws JSONException {
        String type = json.getString("type");
        if (type.equals(Type.RANGE_SELECTOR.name())) {
            return RangeSelector.fromJson(json);
        } else if (type.equals(Type.XPATH_SELECTOR.name())) {
            return XPathSelector.fromJson(json);
        } else if (type.equals(Type.TEXT_POSITION_SELECTOR.name())) {
            return TextPositionSelector.fromJson(json);
        }

        return null;
    }

}
