package io.literal.lib;

import org.json.JSONException;

public interface JsonMapper<In, Out> {
    Out invoke(In in) throws JSONException;
}
