package io.literal.model;

import android.util.JsonReader;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;

import io.literal.lib.JsonReaderParser;

public class XPathSelector<TRefinedBy> extends Selector {

    private final String value;
    private final TRefinedBy[] refinedBy;

    public XPathSelector(@NotNull String value) {
        super(Type.XPATH_SELECTOR);
        this.value = value;
        this.refinedBy = null;
    }

    public XPathSelector(@NotNull String value, TRefinedBy[] refinedBy) {
        super(Type.XPATH_SELECTOR);
        this.value = value;
        this.refinedBy = refinedBy;
    }

    public String getValue() {
        return value;
    }

    public TRefinedBy[] getRefinedBy() {
        return refinedBy;
    }

    public static <TRefinedBy> XPathSelector<TRefinedBy> fromJson (JsonReader reader, JsonReaderParser<TRefinedBy> parseRefinedBySelector) throws IOException {
        String value = null;
        ArrayList<TRefinedBy> refinedBy = null;

        reader.beginObject();
        while (reader.hasNext()) {
            String key = reader.nextName();
            switch (key) {
                case "value":
                    value = reader.nextString();
                    break;
                case "refinedBy":
                    reader.beginArray();
                    while (reader.hasNext()) {
                        TRefinedBy item = parseRefinedBySelector != null ? parseRefinedBySelector.invoke(reader) : null;
                        if (item != null) {
                            if (refinedBy == null) {
                                refinedBy = new ArrayList<>();
                            }
                            refinedBy.add(item);
                        }
                    }
                    reader.endArray();
                    break;
                default:
                    reader.skipValue();
            }
        }
        reader.endObject();

        if (value != null && refinedBy != null) {
            return new XPathSelector(value, (TRefinedBy[]) refinedBy.toArray());
        } else if (value != null) {
            return new XPathSelector(value);
        }
        return null;
    }

    @Override
    public JSONObject toJson() throws JSONException {
        JSONObject result = new JSONObject();

        result.put("type", getType());
        result.put("value", getValue());

        if (getRefinedBy() != null) {
            TRefinedBy[] refinedBy = getRefinedBy();
            JSONObject[] refinedByOutput = new JSONObject[refinedBy.length];
            for (int i = 0; i < getRefinedBy().length; i++) {
                if (refinedBy[i] instanceof Selector) {
                    refinedByOutput[i] = ((Selector) refinedBy[i]).toJson();
                } else {
                    throw new JSONException("Expected refinedBy to be instanceof Selector");
                }
            }
            result.put("refinedBy", new JSONArray(refinedByOutput));

        }

        return result;
    }
}
