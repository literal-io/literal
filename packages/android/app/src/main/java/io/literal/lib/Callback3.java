package io.literal.lib;

public interface Callback3<T1, T2, T3> {
    void invoke(Exception e, T1 data1, T2 data2, T3 data3);
}
