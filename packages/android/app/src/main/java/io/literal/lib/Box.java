package io.literal.lib;

public class Box<T> {
    private T value;

    public Box(T init) {
        this.value = init;
    }

    public T get() {
        return value;
    }

    public void set(T value) {
        this.value = value;
    }
}
