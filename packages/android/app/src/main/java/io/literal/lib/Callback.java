package io.literal.lib;

public interface Callback<T> {
    void invoke(Exception e, T data);
}

