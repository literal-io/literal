package io.literal.lib;

public interface ResultCallback<R, T> {
    R invoke(Exception e, T data);
}

