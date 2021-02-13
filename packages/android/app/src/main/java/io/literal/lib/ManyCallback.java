package io.literal.lib;

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
    }

    public Callback<TError, TData> getCallback(int idx) {
        return new Callback<TError, TData>() {
            @Override
            public void invoke(TError e, TData data) {
                results.set(idx, new Result<>(e, data));

                Stream<Result<TError, TData>> errorStream = results.stream().filter((result) -> result.getError() != null);
                Stream<Result<TError, TData>> dataStream = results.stream().filter((result) -> result.getData() != null);

                if (errorStream.findFirst().isPresent() && !hasResult) {
                    hasResult = true;
                    callback.invoke(errorStream.findFirst().get().getError(), null);
                }

                if (dataStream.count() == size) {
                    hasResult = true;
                    callback.invoke(null, dataStream.map(Result::getData).collect(Collectors.toList()));
                }
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
