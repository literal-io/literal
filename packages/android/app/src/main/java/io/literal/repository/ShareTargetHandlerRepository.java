package io.literal.repository;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.util.Log;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationFromExternalTargetMutation;
import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;
import com.amazonaws.amplify.generated.graphql.GetAnnotationQuery;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobileconnectors.appsync.ClearCacheException;
import com.amazonaws.mobileconnectors.appsync.ClearCacheOptions;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferListener;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferState;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferUtility;
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Error;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONObject;

import java.io.File;
import java.security.NoSuchAlgorithmException;
import java.util.Collections;
import java.util.List;
import java.util.UUID;

import javax.annotation.Nonnull;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.AnnotationCollectionLib;
import io.literal.lib.AnnotationLib;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.Crypto;
import io.literal.lib.WebRoutes;
import io.literal.ui.activity.MainActivity;
import io.literal.ui.activity.ShareTargetHandler;
import io.literal.viewmodel.AuthenticationViewModel;
import type.AnnotationBodyInput;
import type.AnnotationTargetInput;
import type.AnnotationType;
import type.CreateAnnotationFromExternalTargetInput;
import type.CreateAnnotationInput;
import type.ExternalTargetInput;
import type.Format;
import type.Language;
import type.Motivation;
import type.ResourceType;
import type.TextDirection;
import type.TextualBodyInput;
import type.TextualBodyType;
import type.TextualTargetInput;

public class ShareTargetHandlerRepository {
    public interface CreateListener<D> {
        void onAnnotationUri(String uri);
        void onAnnotationCreated(D data);
        void onGraphQLError(List<Error> errors);
        void onError(Exception e);
    }

    public static void createAnnotationFromText(String text, Context context, AuthenticationViewModel authenticationViewModel, CreateListener<CreateAnnotationMutation.Data> listener) {
        String creatorUsername = authenticationViewModel.getUsername().getValue();
        try {
            String valueHash = Crypto.sha256Hex(text);
            String annotationId = WebRoutes.creatorsIdAnnotationId(
                    WebRoutes.getAPIHost(),
                    creatorUsername,
                    valueHash
            );
            String uri = WebRoutes.creatorsIdAnnotationsNewAnnotationId(
                    creatorUsername,
                    valueHash
            );
            listener.onAnnotationUri(uri);

            CreateAnnotationMutation createHighlightMutation = CreateAnnotationMutation
                    .builder()
                    .input(
                            CreateAnnotationInput
                                    .builder()
                                    .context(Collections.singletonList("http://www.w3.org/ns/anno.jsonld"))
                                    .type(Collections.singletonList(AnnotationType.ANNOTATION))
                                    .id(annotationId)
                                    .motivation(Collections.singletonList(Motivation.HIGHLIGHTING))
                                    .creatorUsername(creatorUsername)
                                    .body(
                                            Collections.singletonList(
                                                    AnnotationBodyInput
                                                            .builder()
                                                            .textualBody(
                                                                    TextualBodyInput
                                                                            .builder()
                                                                            .id(AnnotationCollectionLib.makeId(
                                                                                    creatorUsername,
                                                                                    Constants.RECENT_ANNOTATION_COLLECTION_LABEL
                                                                            ))
                                                                            .value(Constants.RECENT_ANNOTATION_COLLECTION_LABEL)
                                                                            .purpose(Collections.singletonList(Motivation.TAGGING))
                                                                            .format(Format.TEXT_PLAIN)
                                                                            .textDirection(TextDirection.LTR)
                                                                            .language(Language.EN_US)
                                                                            .type(TextualBodyType.TEXTUAL_BODY)
                                                                            .build()
                                                            )
                                                            .build()
                                            )
                                    )
                                    .target(Collections.singletonList(
                                            AnnotationTargetInput
                                                    .builder()
                                                    .textualTarget(
                                                            TextualTargetInput
                                                                    .builder()
                                                                    .format(Format.TEXT_PLAIN)
                                                                    .language(Language.EN_US)
                                                                    .processingLanguage(Language.EN_US)
                                                                    .textDirection(TextDirection.LTR)
                                                                    .value(text)
                                                                    .build()
                                                    )
                                                    .build()
                                    ))
                                    .build()
                    )
                    .build();

            AppSyncClientFactory.getInstance(context)
                    .mutate(createHighlightMutation)
                    .enqueue(new GraphQLCall.Callback<CreateAnnotationMutation.Data>() {
                        @Override
                        public void onResponse(@Nonnull Response<CreateAnnotationMutation.Data> highlightResponse) {
                            if (highlightResponse.hasErrors()) {
                                listener.onGraphQLError(highlightResponse.errors());
                                return;
                            }
                            listener.onAnnotationCreated(highlightResponse.data());
                        }

                        @Override
                        public void onFailure(@Nonnull ApolloException e) {
                            listener.onError(e);
                        }
                    });
        } catch (NoSuchAlgorithmException e) {
            listener.onError(e);
        }
    }

    public static void createAnnotationFromImage(Uri imageUri, Context context, AuthenticationViewModel authenticationViewModel, CreateListener<CreateAnnotationFromExternalTargetMutation.Data> listener) {
        String screenshotId = UUID.randomUUID().toString();
        String creatorUsername = authenticationViewModel.getUsername().getValue();
        String creatorIdentityId = authenticationViewModel.getIdentityId().getValue();
        String annotationId = WebRoutes.creatorsIdAnnotationId(
                WebRoutes.getAPIHost(),
                creatorUsername,
                screenshotId
        );
        String uri = WebRoutes.creatorsIdAnnotationsNewAnnotationId(
                creatorUsername,
                screenshotId
        );

        listener.onAnnotationUri(uri);

        JSONObject s3TransferUtilityJson = AppSyncClientFactory
                .getConfiguration(context)
                .optJsonObject("S3TransferUtility");
        String bucket = s3TransferUtilityJson.optString("Bucket");
        String region = s3TransferUtilityJson.optString("Region");


        String filePath = "screenshots/" + screenshotId;
        File file = ContentResolverLib.toFile(context, imageUri, filePath);

        TransferUtility transferUtility = AWSMobileClientFactory.getTransferUtility(context);
        TransferObserver transferObserver = transferUtility.upload(
                bucket,
                "private/" + creatorIdentityId + "/screenshots/" + screenshotId,
                file
        );
        transferObserver.setTransferListener(new TransferListener() {
            @Override
            public void onStateChanged(int id, TransferState state) {
                if (TransferState.COMPLETED != state) {
                    return;
                }
                String uploadURI = "s3://" + transferObserver.getBucket() + "/" + transferObserver.getKey();
                CreateAnnotationFromExternalTargetMutation createAnnotationMutation = CreateAnnotationFromExternalTargetMutation
                        .builder()
                        .input(
                                CreateAnnotationFromExternalTargetInput
                                        .builder()
                                        .creatorUsername(creatorUsername)
                                        .annotationId(annotationId)
                                        .externalTarget(
                                                ExternalTargetInput
                                                        .builder()
                                                        .format(Format.TEXT_PLAIN)
                                                        .language(Language.EN_US)
                                                        .processingLanguage(Language.EN_US)
                                                        .type(ResourceType.IMAGE)
                                                        .id(uploadURI)
                                                        .build()
                                        )
                                        .build()
                        )
                        .build();
                AppSyncClientFactory.getInstance(context)
                        .mutate(createAnnotationMutation)
                        .enqueue(new GraphQLCall.Callback<CreateAnnotationFromExternalTargetMutation.Data>() {
                            @Override
                            public void onResponse(@Nonnull Response<CreateAnnotationFromExternalTargetMutation.Data> annotationResponse) {
                                if (annotationResponse.hasErrors()) {
                                    listener.onGraphQLError(annotationResponse.errors());
                                    return;
                                }
                                listener.onAnnotationCreated(annotationResponse.data());
                            }

                            @Override
                            public void onFailure(@Nonnull ApolloException e) {
                                listener.onError(e);
                            }
                        });
            }

            @Override
            public void onProgressChanged(int id, long bytesCurrent, long bytesTotal) {
                /** noop **/
            }

            @Override
            public void onError(int id, Exception e) {
                listener.onError(e);
            }
        });
    }

    public static void displayAnnotationCreatedNotification(String annotationId, Context context) {
        try {
            // Application may have deleted or changed the annotation.
            AppSyncClientFactory.getInstance(context).clearCaches(ClearCacheOptions.builder().clearQueries().build());
        } catch (ClearCacheException ex) {
            Log.e("refetchAnnotation", "Unable to clear cache: ", ex);
        }

        AppSyncClientFactory.getInstance(context)
                .query(
                        GetAnnotationQuery
                                .builder()
                                .id(annotationId)
                                .creatorUsername(AWSMobileClient.getInstance().getUsername())
                                .build()
                )
                .enqueue(new GraphQLCall.Callback<GetAnnotationQuery.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<GetAnnotationQuery.Data> response) {
                        if (response.hasErrors()) {
                            Log.e(Constants.LOG_TAG, response.errors().toString());
                            return;
                        }
                        GetAnnotationQuery.Data data = response.data();
                        if (data == null) {
                            return;
                        }
                        GetAnnotationQuery.GetAnnotation annotation = data.getAnnotation();
                        if (annotation == null) {
                            return;
                        }

                        Intent intent = new Intent(context, MainActivity.class);
                        Uri annotationUri = Uri.parse(
                                WebRoutes.creatorsIdAnnotationCollectionIdAnnotationId(
                                        AWSMobileClient.getInstance().getUsername(),
                                        Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT,
                                        AnnotationLib.idComponentFromId(annotation.id())
                                )
                        );
                        intent.setData(annotationUri);
                        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

                        String textualTargetValue = null;
                        for (GetAnnotationQuery.Target target : annotation.target()) {
                            GetAnnotationQuery.AsTextualTarget textualTarget = target.asTextualTarget();
                            if (textualTarget != null) {
                                textualTargetValue = textualTarget.value();
                                break;
                            }
                        }

                        if (textualTargetValue != null) {
                            NotificationCompat.Builder builder = new NotificationCompat.Builder(context, Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_ID)
                                    .setSmallIcon(R.drawable.ic_stat_name)
                                    .setColor(Color.BLACK)
                                    .setContentTitle(Constants.NOTIFICATION_ANNOTATION_CREATED_TITLE)
                                    .setStyle(
                                            new NotificationCompat.BigTextStyle().bigText(textualTargetValue)
                                    )
                                    .setContentText(textualTargetValue)
                                    .setPriority(NotificationCompat.PRIORITY_LOW)
                                    .setContentIntent(pendingIntent)
                                    .setAutoCancel(true);
                            NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
                            notificationManager.notify(annotation.id().hashCode(), builder.build());
                        } else {
                            Log.i(Constants.LOG_TAG, "Did not not notify AnnotationCreation: null textualTargetValue");
                        }
                    }

                    @Override
                    public void onFailure(@Nonnull ApolloException e) {

                    }
                });
    }
}