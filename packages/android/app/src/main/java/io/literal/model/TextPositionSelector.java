package io.literal.model;

import android.util.JsonReader;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.util.ArrayList;

import io.literal.lib.JsonReaderParser;

public class TextPositionSelector<TRefinedBy> extends Selector {
    private final int start;
    private final int end;
    private final TRefinedBy[] refinedBy;

    public TextPositionSelector(int start, int end) {
        super(Type.TEXT_POSITION_SELECTOR);
        this.start = start;
        this.end = end;
        this.refinedBy = null;
    }

    public TextPositionSelector(int start, int end, TRefinedBy[] refinedBy) {
        super(Type.TEXT_POSITION_SELECTOR);
        this.start = start;
        this.end = end;
        this.refinedBy = refinedBy;
    }

    public int getStart() {
        return start;
    }

    public int getEnd() {
        return end;
    }

    public TRefinedBy[] getRefinedBy() {
        return refinedBy;
    }

    public static <TRefinedBy> TextPositionSelector<TRefinedBy> fromJson (JsonReader reader, JsonReaderParser<TRefinedBy> parseRefinedBySelector) throws IOException {
        Integer start = null;
        Integer end = null;
        ArrayList<TRefinedBy> refinedBy = null;

        reader.beginObject();
        while (reader.hasNext()) {
            String key = reader.nextName();
            switch (key) {
                case "start":
                    start = reader.nextInt();
                    break;
                case "end":
                    end = reader.nextInt();
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

        if (start != null && end != null && refinedBy != null) {
            return new TextPositionSelector<>(start, end, (TRefinedBy []) refinedBy.toArray());
        } else if (start != null && end != null) {
            return new TextPositionSelector<>(start, end);
        }

        return null;
    }

    @Override
    public JSONObject toJson() throws JSONException {
        JSONObject result = new JSONObject();

        result.put("type", getType());
        result.put("start", getStart());
        result.put("end", getEnd());

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
