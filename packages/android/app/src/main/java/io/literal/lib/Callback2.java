package io.literal.lib;

public interface Callback2<T1, T2> {
    void invoke(Exception e, T1 data1, T2 data2);
}
