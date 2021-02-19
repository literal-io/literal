package io.literal.lib;

import java.util.Arrays;

public class ArrayUtil {
    public static <T> T[] add(T[] arr, T item) {
        int idx = arr.length;
        T[] copy = Arrays.copyOf(arr, idx + 1);
        copy[idx] = item;

        return copy;
    }
}
