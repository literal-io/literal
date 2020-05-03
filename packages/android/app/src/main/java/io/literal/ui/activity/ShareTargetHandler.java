package io.literal.ui.activity;

import io.literal.R;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.ui.view.WebView;
import io.literal.lib.WebEvent;

import type.CreateHighlightFromScreenshotInput;
import type.CreateHighlightInput;
import type.CreateScreenshotInput;
import type.S3ObjectInput;

import javax.annotation.Nonnull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import com.amazonaws.amplify.generated.graphql.CreateHighlightFromScreenshotMutation;
import com.amazonaws.amplify.generated.graphql.CreateHighlightMutation;
import com.amazonaws.amplify.generated.graphql.CreateScreenshotMutation;

import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONObject;

import java.io.File;
import java.util.UUID;
import java.util.concurrent.CountDownLatch;

public class ShareTargetHandler extends AppCompatActivity {

    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_target_handler);

        webView = findViewById(R.id.webview);

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

            if (Intent.ACTION_SEND.equals(action) && type != null && type.startsWith("image/")) {
                handleSendImage(intent);
            } else if (Intent.ACTION_SEND.equals(action) && type != null && type.equals("text/plain")) {
                handleSendText(intent);
            } else {
                handleSendNotSupported();
            }
        }

    }

    void handleSendText(Intent intent) {
        String text = intent.getStringExtra(Intent.EXTRA_TEXT);

        String highlightId = UUID.randomUUID().toString();
        CreateHighlightMutation createHighlightMutation = CreateHighlightMutation
                .builder()
                .input(
                        CreateHighlightInput
                                .builder()
                                .id(highlightId)
                                .text(text)
                                .build()
                )
                .build();

        AppSyncClientFactory.getInstance(this)
                .mutate(createHighlightMutation)
                .enqueue(createHighlightCallback.getCallback());

        try {
            final CreateHighlightMutation.Data highlightResult = createHighlightCallback.awaitResult();
            if (highlightResult == null) {
                // TODO: handle errors causing screenshot to not be created.
                Log.i("Literal", "highlightResult is null");
                finish();
                return;
            }

            webView.onWebEvent(new WebView.WebEventCallback() {
                @Override
                public void onWebEvent(WebEvent event) {
                    switch (event.getType()) {
                        case WebEvent.TYPE_ACTIVITY_FINISH:
                            CreateHighlightMutation.CreateHighlight highlight = highlightResult.createHighlight();
                            if (highlight != null) {
                                handleDisplayNotification(highlight.id(), highlight.text());
                            }
                            finish();
                    }
                }
            });

            webView.loadUrl(Constants.WEB_HOST + "/notes/" + highlightId);
        } catch (InterruptedException ex) {
            // TODO: handle errors causing screenshot to not be created.
            Log.e(Constants.LOG_TAG, "createHighlight failed", ex);
            finish();
        }
    }

    void handleSendImage(Intent intent) {
        JSONObject s3TransferUtilityJson = AppSyncClientFactory
                .getConfiguration(this)
                .optJsonObject("S3TransferUtility");
        String bucket = s3TransferUtilityJson.optString("Bucket");
        String region = s3TransferUtilityJson.optString("Region");

        String screenshotId = UUID.randomUUID().toString();
        Uri imageUri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
        String filePath = "screenshots/" + screenshotId;
        File file = ContentResolverLib.toFile(this, imageUri, filePath);

        CreateScreenshotMutation createScreenshotMutation = CreateScreenshotMutation
                .builder()
                .input(
                        CreateScreenshotInput.builder()
                                .id(screenshotId)
                                .file(
                                        S3ObjectInput.builder()
                                                .bucket(bucket)
                                                .key("public/screenshot/" + screenshotId)
                                                .region(region)
                                                .localUri(file.getAbsolutePath())
                                                .mimeType(intent.getType())
                                                .build()
                                )
                                .build()
                )
                .build();
        AppSyncClientFactory.getInstance(this)
                .mutate(createScreenshotMutation)
                .enqueue(createScreenshotCallback.getCallback());

        // TODO: handle errors causing screenshot to not be created.
        CreateScreenshotMutation.Data screenshotResult = null;
        try {
            screenshotResult = createScreenshotCallback.awaitResult();
        } catch (InterruptedException e) {}
        if (screenshotResult == null) {
            Log.i("Literal", "screenshotResult is null");
            return;
        }

        String highlightId = UUID.randomUUID().toString();
        CreateHighlightFromScreenshotMutation createHighlightMutation = CreateHighlightFromScreenshotMutation.builder()
                .input(
                        CreateHighlightFromScreenshotInput.builder()
                                .id(highlightId)
                                .screenshotId(screenshotId)
                                .build()
                )
                .build();

        AppSyncClientFactory.getInstance(this)
                .mutate(createHighlightMutation)
                .enqueue(createHighlightFromScreenshotCallback.getCallback());

        try {
            final CreateHighlightFromScreenshotMutation.Data highlightResult = createHighlightFromScreenshotCallback.awaitResult();

            // TODO: handle errors causing screenshot to not be created.
            if (highlightResult == null) {
                Log.i("Literal", "highlightResult is null");
                return;
            }

            webView.onWebEvent(new WebView.WebEventCallback() {
                @Override
                public void onWebEvent(WebEvent event) {
                    switch (event.getType()) {
                        case WebEvent.TYPE_ACTIVITY_FINISH:
                            CreateHighlightFromScreenshotMutation.CreateHighlightFromScreenshot highlight = highlightResult.createHighlightFromScreenshot();
                            if (highlight != null) {
                                handleDisplayNotification(highlight.id(), highlight.text());
                            }
                            finish();
                    }
                }
            });

            webView.loadUrl(Constants.WEB_HOST + "/notes/" + highlightId);
        } catch (InterruptedException ex) {
            // TODO: handle errors causing screenshot to not be created.
            Log.e(Constants.LOG_TAG, "createHighlightFromScreenshot failed", ex);
            finish();
        }

    }

    private void handleSendNotSupported() {

        // TODO: implement fallback handling, e.g. display a "This does not look like a screenshot" UI
    }

    private void handleDisplayNotification(String noteId, String noteText) {
        Intent intent = new Intent(this, MainActivity.class);
        intent.setData(Uri.parse(Constants.WEB_HOST + "/notes/" + noteId));
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, intent, 0);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(ShareTargetHandler.this, Constants.NOTIFICATION_CHANNEL_NOTE_CREATED_ID)
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(Constants.NOTIFICATION_NOTE_CREATED_TITLE)
                .setStyle(
                        new NotificationCompat.BigTextStyle().bigText(noteText)
                )
                .setContentText(noteText)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        notificationManager.notify(noteId.hashCode(), builder.build());
    }

    @Override
    protected void onSaveInstanceState(Bundle outState)
    {
        super.onSaveInstanceState(outState);
        if (this.webView != null) {
            webView.saveState(outState);
        }
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState)
    {
        super.onRestoreInstanceState(savedInstanceState);
        if (this.webView != null) {
            this.webView.restoreState(savedInstanceState);
        }
    }

    private LatchedGraphQLCallback<CreateScreenshotMutation.Data> createScreenshotCallback = new LatchedGraphQLCallback<>();
    private LatchedGraphQLCallback<CreateHighlightFromScreenshotMutation.Data> createHighlightFromScreenshotCallback = new LatchedGraphQLCallback<>();
    private LatchedGraphQLCallback<CreateHighlightMutation.Data> createHighlightCallback = new LatchedGraphQLCallback<>();

    private class LatchedGraphQLCallback<T> extends CountDownLatch {

        private volatile T result;

        public LatchedGraphQLCallback() {
            super(1);
        }

        public LatchedGraphQLCallback(int num) {
            super(num);
        }

        public T awaitResult() throws InterruptedException {
            this.await();
            return result;
        }

        public GraphQLCall.Callback<T> getCallback() {
            return new GraphQLCall.Callback<T>() {
                @Override
                public void onResponse(@Nonnull Response<T> response) {
                    result = response.data();
                    countDown();
                }

                @Override
                public void onFailure(@Nonnull ApolloException e) {
                    Log.e("Literal", "LatchedGraphQLCallback", e);
                    countDown();
                }
            };
        }
    }
}
