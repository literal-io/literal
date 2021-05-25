package io.literal.repository;

import android.content.Context;
import android.util.Log;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;
import com.amazonaws.amplify.generated.graphql.DeleteAnnotationMutation;
import com.amazonaws.amplify.generated.graphql.GetAnnotationQuery;
import com.amazonaws.amplify.generated.graphql.PatchAnnotationMutation;
import com.amazonaws.mobileconnectors.appsync.AWSAppSyncClient;
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Operation;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;

import javax.annotation.Nonnull;

import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Callback;
import io.literal.lib.ManyCallback;
import io.literal.model.Annotation;
import type.CreateAnnotationInput;
import type.DeleteAnnotationInput;
import type.PatchAnnotationInput;

public class AnnotationRepository {
    public static void getAnnotationQuery(AWSAppSyncClient appSyncClient, GetAnnotationQuery query, Callback<ApolloException, GetAnnotationQuery.Data> callback) {
        appSyncClient
                .query(query)
                .enqueue(new GraphQLCall.Callback<GetAnnotationQuery.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<GetAnnotationQuery.Data> response) {
                        if (response.hasErrors()) {
                            response.errors().forEach((error -> ErrorRepository.captureException(new Exception(error.message()))));
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

    public static void patchAnnotationMutation(AWSAppSyncClient appSyncClient, PatchAnnotationInput input, Callback<ApolloException, PatchAnnotationMutation.Data> callback) {
        try {
            JSONObject properties = new JSONObject();
            JSONObject operationVariables = new JSONObject();
            operationVariables.put("id", input.id());
            properties.put("operationName", "PatchAnnotation");
            properties.put("operationVariables", operationVariables);

            AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_GRAPH_QL_OPERATION, properties);
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
        }

        appSyncClient
                .mutate(PatchAnnotationMutation.builder().input(input).build())
                .enqueue(new GraphQLCall.Callback<PatchAnnotationMutation.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<PatchAnnotationMutation.Data> response) {
                        if (response.hasErrors()) {
                            response.errors().forEach((error -> {
                                ErrorRepository.captureException(new Exception(error.message()));
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

    public static void deleteAnnotationMutation(AWSAppSyncClient appSyncClient, DeleteAnnotationInput input, Callback<ApolloException, DeleteAnnotationMutation.Data> callback) {
        try {
            JSONObject properties = new JSONObject();
            JSONObject operationVariables = new JSONObject();
            operationVariables.put("id", input.id());
            properties.put("operationName", "DeleteAnnotation");
            properties.put("operationVariables", operationVariables);

            AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_GRAPH_QL_OPERATION, properties);
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
        }

        appSyncClient
                .mutate(DeleteAnnotationMutation.builder().input(input).build())
                .enqueue(new GraphQLCall.Callback<DeleteAnnotationMutation.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<DeleteAnnotationMutation.Data> response) {
                        if (response.hasErrors()) {
                            response.errors().forEach((error -> ErrorRepository.captureException(new Exception(error.message()))));
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

    public static void createAnnotations(AWSAppSyncClient appSyncClient, Annotation[] annotations, Callback<ApolloException, List<CreateAnnotationMutation.Data>> callback) {
        ManyCallback<ApolloException, CreateAnnotationMutation.Data> manyCallback = new ManyCallback<>(annotations.length, callback);

        for (int i = 0; i < annotations.length; i++) {
            Callback<ApolloException, CreateAnnotationMutation.Data> innerCallback = manyCallback.getCallback(i);
            CreateAnnotationInput input = annotations[i].toCreateAnnotationInput();

            try {
                JSONObject properties = new JSONObject();
                JSONObject operationVariables = new JSONObject();
                operationVariables.put("id", input.id());
                properties.put("operationName", "CreateAnnotation");
                properties.put("operationVariables", operationVariables);

                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_GRAPH_QL_OPERATION, properties);
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            }

            appSyncClient
                    .mutate(
                            CreateAnnotationMutation.builder()
                                    .input(input)
                                    .build()
                    )
                    .enqueue(new GraphQLCall.Callback<CreateAnnotationMutation.Data>() {
                        @Override
                        public void onResponse(@Nonnull Response<CreateAnnotationMutation.Data> response) {
                            if (response.hasErrors()) {
                                response.errors().forEach((error -> ErrorRepository.captureException(new Exception(error.message()))));
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
