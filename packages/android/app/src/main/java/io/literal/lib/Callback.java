package io.literal.lib;

public interface Callback<TError, TData> {
    void invoke(TError e, TData data);
}

