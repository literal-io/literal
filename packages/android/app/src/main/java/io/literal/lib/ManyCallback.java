package io.literal.lib;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public class ManyCallback<TError, TData> {

    private int size;
    private ArrayList<Result<TError, TData>> results;
    private Callback<TError, List<TData>> callback;
    private boolean hasResult;


    public ManyCallback(int size, Callback<TError, List<TData>> callback) {
        this.size = size;
        this.results = new ArrayList<>(size);
        this.callback = callback;
        for (int i = 0; i < size; i++) {
            this.results.add(null);
        }
    }

    public Callback<TError, TData> getCallback(int idx) {
        return (e, data) -> {
            results.set(idx, new Result<>(e, data));

            List<TError> errorResult = results.stream().filter((result) -> result.getError() != null).map(Result::getError).collect(Collectors.toList());
            List<TData> dataResult = results.stream().filter((result) -> result.getData() != null).map(Result::getData).collect(Collectors.toList());

            if (errorResult.size() > 0 && !hasResult) {
                hasResult = true;
                callback.invoke(errorResult.get(0), null);
            } else if (dataResult.size() == size) {
                hasResult = true;
                callback.invoke(null, dataResult);
            }
        };
    }

    public class Result<TError, TData> {
        private final TError error;
        private final TData data;

        public Result(TError error, TData data) {
            this.error = error;
            this.data = data;
        }

        public TData getData() {
            return data;
        }

        public TError getError() {
            return error;
        }
    }
}
