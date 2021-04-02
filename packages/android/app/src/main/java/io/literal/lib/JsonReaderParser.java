package io.literal.lib;

import android.util.JsonReader;

import java.io.IOException;

public interface JsonReaderParser<T> {
    public T invoke(JsonReader reader) throws IOException;
}
