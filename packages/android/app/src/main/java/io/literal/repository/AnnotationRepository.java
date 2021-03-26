package io.literal.repository;

import android.content.Context;
import android.util.Log;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;
import com.amazonaws.amplify.generated.graphql.DeleteAnnotationMutation;
import com.amazonaws.amplify.generated.graphql.GetAnnotationQuery;
import com.amazonaws.amplify.generated.graphql.PatchAnnotationMutation;
import com.amazonaws.mobileconnectors.appsync.AWSAppSyncClient;
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONException;

import java.util.List;

import javax.annotation.Nonnull;

import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Callback;
import io.literal.lib.ManyCallback;
import io.literal.model.Annotation;
import type.DeleteAnnotationInput;
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

    public static void deleteAnnotationMutation(Context context, DeleteAnnotationInput input, Callback<ApolloException, DeleteAnnotationMutation.Data> callback) {
        AppSyncClientFactory.getInstance(context)
                .mutate(DeleteAnnotationMutation.builder().input(input).build())
                .enqueue(new GraphQLCall.Callback<DeleteAnnotationMutation.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<DeleteAnnotationMutation.Data> response) {
                        if (response.hasErrors()) {
                            response.errors().forEach((error -> {
                                Log.d("AnnotationRepository", "deleteAnnotationMutation error: " + error.message());
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


    public static void createAnnotations(Context context, Annotation[] annotations, Callback<ApolloException, List<CreateAnnotationMutation.Data>> callback) {
        ManyCallback<ApolloException, CreateAnnotationMutation.Data> manyCallback = new ManyCallback<>(annotations.length, callback);
        AWSAppSyncClient appSyncClient = AppSyncClientFactory.getInstance(context);

        for (int i = 0; i < annotations.length; i++) {
            try {
                Log.i("ShareTargetHandlerRepository", "creating annotation: " + annotations[i].toJson());
            } catch (JSONException e) {
                Log.d("ShareTargetHandlerRepository", "serialization error", e);
            }
            Callback<ApolloException, CreateAnnotationMutation.Data> innerCallback = manyCallback.getCallback(i);
            appSyncClient
                    .mutate(
                            CreateAnnotationMutation.builder()
                                    .input(annotations[i].toCreateAnnotationInput())
                                    .build()

                    )
                    .enqueue(new GraphQLCall.Callback<CreateAnnotationMutation.Data>() {
                        @Override
                        public void onResponse(@Nonnull Response<CreateAnnotationMutation.Data> response) {
                            if (response.hasErrors()) {
                                response.errors().forEach((error -> {
                                    Log.d("ShareTargetHandlerRepository", "createAnnotations error: " + error.message());
                                }));
                                innerCallback.invoke(new ApolloException("Server Error"), null);
                            } else {
                                innerCallback.invoke(null, response.data());
                            }
                        }
                        @Override
                        public void onFailure(@Nonnull ApolloException e) {
                            innerCallback.invoke(e, null);
                        }
                    });
        }
    }
}
