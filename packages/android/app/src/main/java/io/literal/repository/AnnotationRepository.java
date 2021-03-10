package io.literal.repository;

import android.content.Context;
import android.util.Log;

import com.amazonaws.amplify.generated.graphql.GetAnnotationQuery;
import com.amazonaws.amplify.generated.graphql.PatchAnnotationMutation;
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import javax.annotation.Nonnull;

import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Callback;
import type.PatchAnnotationInput;

public class AnnotationRepository {
    public static void patchAnnotationMutation(Context context, PatchAnnotationInput input, Callback<ApolloException, PatchAnnotationMutation.Data> callback) {
        AppSyncClientFactory.getInstance(context)
                .mutate(PatchAnnotationMutation.builder().input(input).build())
                .enqueue(new GraphQLCall.Callback<PatchAnnotationMutation.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<PatchAnnotationMutation.Data> response) {
                        if (response.hasErrors()) {
                            response.errors().forEach((error -> {
                                Log.d("PatchAnnotationMutationRepository", "patchAnnotationMutation error: " + error.message());
                            }));
                            callback.invoke(new ApolloException("Server Error"), null);
                        } else {
                            callback.invoke(null, response.data());
                        }
                    }

                    @Override
                    public void onFailure(@Nonnull ApolloException e) {
                        callback.invoke(e, null);
                    }
                });
    }

    public static void getAnnotationQuery(Context context, GetAnnotationQuery query, Callback<ApolloException, GetAnnotationQuery.Data> callback) {
        AppSyncClientFactory.getInstance(context)
                .query(query)
                .enqueue(new GraphQLCall.Callback<GetAnnotationQuery.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<GetAnnotationQuery.Data> response) {
                        if (response.hasErrors()) {
                            response.errors().forEach((error -> {
                                Log.d("GetAnnotation", "getAnnotation error: " + error.message());
                            }));
                            callback.invoke(new ApolloException("Server Error"), null);
                        } else {
                            callback.invoke(null, response.data());
                        }
                    }

                    @Override
                    public void onFailure(@Nonnull ApolloException e) {
                        callback.invoke(e, null);
                    }
                });
    }
}
