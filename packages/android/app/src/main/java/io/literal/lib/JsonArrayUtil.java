package io.literal.lib;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;

public class JsonArrayUtil {
    public static String[] parseJsonStringArray(JSONArray input) throws JSONException {
        String[] output = new String[input.length()];
        for (int i = 0; i < input.length(); i++) {
            output[i] = input.getString(i);
        }
        return output;
    }

    public static <T> T[] parseJsonObjectArray(JSONArray input, T[] output, JsonMapper<JSONObject, T> mapper) throws JSONException {
        T[] sizedOutput = Arrays.copyOf(output, input.length());
        for (int i = 0; i < input.length(); i++) {
            sizedOutput[i] = mapper.invoke(input.getJSONObject(i));
        }
        return sizedOutput;
    }

    public static <T> JSONArray stringifyObjectArray(T[] input, JsonMapper<T, JSONObject> mapper) throws JSONException {
        JSONArray output = new JSONArray();
        for (int i = 0; i < input.length; i++) {
            output.put(mapper.invoke(input[i]));
        }
        return output;
    }
}
