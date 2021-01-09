package io.literal.lib.jsonparser;

import android.util.JsonReader;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;

import java.io.IOException;
import java.io.StringReader;

import type.RangeSelectorInput;
import type.XPathSelectorInput;

// https://developer.android.com/reference/android/util/JsonReader
class DOMSelectionToAnnotationTarget {
    /**
    public static RangeSelectorInput parse(String json) throws IOException {
        JsonReader reader = new JsonReader(new StringReader(json));
        try {
            return parseRangeSelector(reader);
        } finally {
            reader.close();
        }
    }

    public static RangeSelectorInput parseRangeSelector(JsonReader reader) throws IOException {

        XPathSelectorInput endSelector;
        XPathSelectorInput startSelector;

        reader.beginObject();
        while (reader.hasNext()) {
            String key = reader.nextName();
            switch (key) {
                case "endSelector":
                    endSelector = parseXPathSelector(reader);
            }
        }
    }

    public static XPathSelectorInput parseXPathSelector(JsonReader reader) {

    }

     **/
}