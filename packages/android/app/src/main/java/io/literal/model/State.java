package io.literal.model;

import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import type.StateInput;

public class State {
    public enum Type {
        TIME_STATE
    }

    protected final Type type;

    public State(Type type) { this.type = type; }

    public Type getType() { return this.type; }

    public JSONObject toJson() throws JSONException {
        throw new JSONException("Serialization not implemented.");
    }

    public StateInput toStateInput() {
        Log.d("State", "Attempted to call toAnnotationTarget, but not implemented.");
        return null;
    }

    public static State fromJson(JSONObject json) throws JSONException {
        String type = json.getString("type");
        if (type.equals(Type.TIME_STATE.name())) {
            return TimeState.fromJson(json);
        }

        return null;
    }
}
