package io.literal.ui.activity;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationFromExternalTargetMutation;
import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;
import com.amazonaws.amplify.generated.graphql.GetAnnotationForNotificationQuery;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobileconnectors.appsync.ClearCacheException;
import com.amazonaws.mobileconnectors.appsync.ClearCacheOptions;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferListener;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferState;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferUtility;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONObject;

import java.io.File;
import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.Collections;
import java.util.UUID;

import javax.annotation.Nonnull;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.Crypto;
import io.literal.lib.WebEvent;
import io.literal.ui.view.WebView;
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
import type.TextualTargetInput;

public class ShareTargetHandler extends AppCompatActivity {

    private WebView webView;
    private ViewGroup splash;
    private ViewGroup layout;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_target_handler);

        webView = findViewById(R.id.webview);
        webView.setVisibility(View.INVISIBLE);
        splash = findViewById(R.id.splash);
        layout = (ViewGroup) splash.getParent();

        AWSMobileClientFactory.initializeClient(this, new Callback<UserStateDetails>() {
            @Override
            public void onResult(UserStateDetails result) {
                switch (result.getUserState()) {
                    case SIGNED_IN:
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                handleSignedIn(savedInstanceState);
                            }
                        });
                        break;
                    default:
                        handleSignedOut();
                }
            }

            @Override
            public void onError(Exception e) {
                Log.e(Constants.LOG_TAG, "Unable to initializeClient: ", e);
                handleSignedOut();
            }
        });
    }

    private void handleSignedIn(Bundle savedInstanceState) {
        webView.initialize(ShareTargetHandler.this);
        webView.requestFocus();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel chan = new NotificationChannel(
                    Constants.NOTIFICATION_CHANNEL_NOTE_CREATED_ID,
                    Constants.NOTIFICATION_CHANNEL_NOTE_CREATED_NAME,
                    NotificationManager.IMPORTANCE_LOW
            );
            chan.setDescription(Constants.NOTIFICATION_CHANNEL_NOTE_CREATED_DESCRIPTION);
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(chan);
            }
        }

        if (savedInstanceState == null) {
            Intent intent = getIntent();
            String action = intent.getAction();
            String type = intent.getType();

            // Start initializing the WebView while we're processing the share data.
            webView.onWebEvent(new WebEvent.Callback(this, webView) {
                @Override
                public void onWebEvent(WebEvent event) {
                    super.onWebEvent(event);
                    switch (event.getType()) {
                        case WebEvent.TYPE_ACTIVITY_FINISH:
                            finish();
                    }
                }
            });
            webView.onPageFinished(new WebView.PageFinishedCallback() {
                @Override
                public void onPageFinished(android.webkit.WebView view, String Url) {
                    view.setVisibility(View.VISIBLE);
                    layout.removeView(splash);
                }
            });

            if (Intent.ACTION_SEND.equals(action) && type != null && type.startsWith("image/")) {
                handleSendImage(intent);
            } else if (Intent.ACTION_SEND.equals(action) && type != null && type.equals("text/plain")) {
                handleSendText(intent);
            } else {
                handleSendNotSupported();
            }
        }
    }

    private void handleSignedOut() {
        Intent intent = new Intent(this, MainActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_TASK_ON_HOME);
        this.startActivity(intent);
        this.finish();
    }

    private void handleSendText(Intent intent) {

        String text = intent.getStringExtra(Intent.EXTRA_TEXT);

        String creatorUsername = AWSMobileClient.getInstance().getUsername();
        try {
            String valueHash = Crypto.sha256Hex(text);
            String annotationId = "https://literal.io/creators/" + creatorUsername + "/annotations/" + valueHash;
            webView.loadUrl(Constants.WEB_HOST + "/notes/new?id=" + annotationId);
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

            AppSyncClientFactory.getInstance(this)
                    .mutate(createHighlightMutation)
                    .enqueue(new GraphQLCall.Callback<CreateAnnotationMutation.Data>() {
                        @Override
                        public void onResponse(@Nonnull Response<CreateAnnotationMutation.Data> highlightResponse) {
                            if (highlightResponse.hasErrors()) {
                                // Error is handled in WebView.
                                Log.e(Constants.LOG_TAG, highlightResponse.errors().toString());
                                return;
                            }


                            final CreateAnnotationMutation.Data annotationData = highlightResponse.data();
                            if (annotationData == null) {
                                // Error is handled in WebView
                                Log.i(Constants.LOG_TAG, "highlightResult is null");
                                return;
                            }

                            webView.onWebEvent(new WebEvent.Callback(ShareTargetHandler.this, webView) {
                                @Override
                                public void onWebEvent(WebEvent event) {
                                    super.onWebEvent(event);
                                    switch (event.getType()) {
                                        case WebEvent.TYPE_ACTIVITY_FINISH:
                                            CreateAnnotationMutation.CreateAnnotation annotation = annotationData.createAnnotation();
                                            if (annotation != null) {
                                                handleDisplayNotification(annotation.annotation().id());
                                            }
                                            finish();
                                    }
                                }
                            });
                        }

                        @Override
                        public void onFailure(@Nonnull ApolloException e) {

                        }
                    });
        } catch (NoSuchAlgorithmException ex) {
            Log.e(Constants.LOG_TAG, "No such algorithm", ex);
            /** error is handled in webview **/
        }
    }

    private void handleSendImage(Intent intent) {

        String screenshotId = UUID.randomUUID().toString();
        String creatorUsername = AWSMobileClient.getInstance().getUsername();
        String annotationId = "https://literal.io/creators/" + creatorUsername + "/annotations/" + screenshotId;
        webView.loadUrl(Constants.WEB_HOST + "/notes/new?id=" + annotationId);

        JSONObject s3TransferUtilityJson = AppSyncClientFactory
                .getConfiguration(this)
                .optJsonObject("S3TransferUtility");
        String bucket = s3TransferUtilityJson.optString("Bucket");
        String region = s3TransferUtilityJson.optString("Region");

        Uri imageUri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
        String filePath = "screenshots/" + screenshotId;
        File file = ContentResolverLib.toFile(this, imageUri, filePath);

        TransferUtility transferUtility = AWSMobileClientFactory.getTransferUtility(getApplicationContext());
        TransferObserver transferObserver = transferUtility.upload(
                bucket,
                "public/screenshot/" + screenshotId,
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
                AppSyncClientFactory.getInstance(ShareTargetHandler.this)
                        .mutate(createAnnotationMutation)
                        .enqueue(new GraphQLCall.Callback<CreateAnnotationFromExternalTargetMutation.Data>() {
                            @Override
                            public void onResponse(@Nonnull Response<CreateAnnotationFromExternalTargetMutation.Data> annotationResponse) {
                                if (annotationResponse.hasErrors()) {
                                    // Error is handled in WebView.
                                    Log.e(Constants.LOG_TAG, annotationResponse.errors().toString());
                                    return;
                                }

                                final CreateAnnotationFromExternalTargetMutation.Data annotationData = annotationResponse.data();
                                if (annotationData == null) {
                                    // Error is handled in WebView.
                                    Log.i("Literal", "highlightResult is null");
                                    return;
                                }
                                webView.onWebEvent(new WebEvent.Callback(ShareTargetHandler.this, webView) {
                                    @Override
                                    public void onWebEvent(WebEvent event) {
                                        super.onWebEvent(event);
                                        switch (event.getType()) {
                                            case WebEvent.TYPE_ACTIVITY_FINISH:
                                                CreateAnnotationFromExternalTargetMutation.CreateAnnotationFromExternalTarget annotation = annotationData.createAnnotationFromExternalTarget();
                                                if (annotation != null) {
                                                    handleDisplayNotification(annotation.id());
                                                }
                                                finish();
                                        }
                                    }
                                });
                            }

                            @Override
                            public void onFailure(@Nonnull ApolloException e) {
                                // Error is handled in WebView.
                                Log.e(Constants.LOG_TAG, "ApolloException", e);
                            }
                        });
            }

            @Override
            public void onProgressChanged(int id, long bytesCurrent, long bytesTotal) {
                /** noop **/
            }

            @Override
            public void onError(int id, Exception ex) {
                Log.e(Constants.LOG_TAG, "TransferUtility", ex);
                /** error is handled in webview **/
            }
        });
    }

    private void handleSendNotSupported() {
        // TODO: implement fallback handling, e.g. display a "This does not look like a screenshot" UI
    }

    private void handleDisplayNotification(String annotationId) {
        try {
            // NewHighlightFromShare may have deleted or changed the highlight.
            AppSyncClientFactory.getInstance(this).clearCaches(ClearCacheOptions.builder().clearQueries().build());
        } catch (ClearCacheException ex) {
            Log.e(Constants.LOG_TAG, "Unable to clear cache: ", ex);
        }

        AppSyncClientFactory.getInstance(this)
                .query(
                        GetAnnotationForNotificationQuery
                                .builder()
                                .id(annotationId)
                                .creatorUsername(AWSMobileClient.getInstance().getUsername())
                                .build()
                )
                .enqueue(new GraphQLCall.Callback<GetAnnotationForNotificationQuery.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<GetAnnotationForNotificationQuery.Data> response) {
                        if (response.hasErrors()) {
                            Log.e(Constants.LOG_TAG, response.errors().toString());
                            return;
                        }
                        GetAnnotationForNotificationQuery.Data data = response.data();
                        if (data == null) {
                            return;
                        }
                        GetAnnotationForNotificationQuery.GetAnnotation annotation = data.getAnnotation();
                        if (annotation == null) {
                            return;
                        }

                        Intent intent = new Intent(ShareTargetHandler.this, MainActivity.class);
                        intent.setData(Uri.parse(Constants.WEB_HOST + "/notes?id=" + annotation.id()));
                        PendingIntent pendingIntent = PendingIntent.getActivity(ShareTargetHandler.this, 0, intent, 0);

                        String textualTargetValue = null;
                        for (GetAnnotationForNotificationQuery.Target target: annotation.target()) {
                            GetAnnotationForNotificationQuery.AsTextualTarget textualTarget = target.asTextualTarget();
                            if (textualTarget != null) {
                                textualTargetValue = textualTarget.value();
                                break;
                            }
                        }

                        if (textualTargetValue != null) {
                            NotificationCompat.Builder builder = new NotificationCompat.Builder(ShareTargetHandler.this, Constants.NOTIFICATION_CHANNEL_NOTE_CREATED_ID)
                                    .setSmallIcon(R.drawable.ic_stat_name)
                                    .setColor(Color.BLACK)
                                    .setContentTitle(Constants.NOTIFICATION_NOTE_CREATED_TITLE)
                                    .setStyle(
                                            new NotificationCompat.BigTextStyle().bigText(textualTargetValue)
                                    )
                                    .setContentText(textualTargetValue)
                                    .setPriority(NotificationCompat.PRIORITY_LOW)
                                    .setContentIntent(pendingIntent)
                                    .setAutoCancel(true);
                            NotificationManagerCompat notificationManager = NotificationManagerCompat.from(ShareTargetHandler.this);
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

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (this.webView != null) {
            webView.saveState(outState);
        }
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        if (this.webView != null) {
            this.webView.restoreState(savedInstanceState);
        }
    }
}
