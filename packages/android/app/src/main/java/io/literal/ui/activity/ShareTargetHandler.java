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

import com.amazonaws.amplify.generated.graphql.CreateHighlightFromScreenshotMutation;
import com.amazonaws.amplify.generated.graphql.CreateHighlightMutation;
import com.amazonaws.amplify.generated.graphql.CreateScreenshotMutation;
import com.amazonaws.amplify.generated.graphql.GetHighlightQuery;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobileconnectors.appsync.ClearCacheException;
import com.amazonaws.mobileconnectors.appsync.ClearCacheOptions;
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONObject;

import java.io.File;
import java.util.UUID;
import java.util.concurrent.CountDownLatch;

import javax.annotation.Nonnull;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.WebEvent;
import io.literal.ui.view.WebView;
import type.CreateHighlightFromScreenshotInput;
import type.CreateHighlightInput;
import type.CreateScreenshotInput;
import type.S3ObjectInput;

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
        String highlightId = UUID.randomUUID().toString();
        webView.loadUrl(Constants.WEB_HOST + "/notes/new?id=" + highlightId);

        String text = intent.getStringExtra(Intent.EXTRA_TEXT);
        CreateHighlightMutation createHighlightMutation = CreateHighlightMutation
                .builder()
                .input(
                        CreateHighlightInput
                                .builder()
                                .id(highlightId)
                                .text(text)
                                .owner(AWSMobileClient.getInstance().getUsername())
                                .build()
                )
                .build();

        AppSyncClientFactory.getInstance(this)
                .mutate(createHighlightMutation)
                .enqueue(new GraphQLCall.Callback<CreateHighlightMutation.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<CreateHighlightMutation.Data> highlightResponse) {
                        if (highlightResponse.hasErrors()) {
                            // Error is handled in WebView.
                            Log.e(Constants.LOG_TAG, highlightResponse.errors().toString());
                            return;
                        }


                        final CreateHighlightMutation.Data highlightData = highlightResponse.data();
                        if (highlightData == null) {
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
                                        CreateHighlightMutation.CreateHighlight highlight = highlightData.createHighlight();
                                        if (highlight != null) {
                                            handleDisplayNotification(highlight.id());
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
    }

    private void handleSendImage(Intent intent) {
        String highlightId = UUID.randomUUID().toString();
        webView.loadUrl(Constants.WEB_HOST + "/notes/new?id=" + highlightId);

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
                                .owner(AWSMobileClient.getInstance().getUsername())
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
                .enqueue(new GraphQLCall.Callback<CreateScreenshotMutation.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<CreateScreenshotMutation.Data> screenshotResponse) {
                        if (screenshotResponse.hasErrors()) {
                            // Error is handled in WebView
                            Log.e(Constants.LOG_TAG, screenshotResponse.errors().toString());
                            return;
                        }
                        CreateScreenshotMutation.Data screenshotData = screenshotResponse.data();
                        if (screenshotData == null) {
                            // Error is handled in WebView
                            Log.i("Literal", "screenshotResult is null");
                            return;
                        }

                        CreateHighlightFromScreenshotMutation createHighlightMutation = CreateHighlightFromScreenshotMutation.builder()
                                .input(
                                        CreateHighlightFromScreenshotInput.builder()
                                                .id(highlightId)
                                                .screenshotId(screenshotId)
                                                .owner(AWSMobileClient.getInstance().getUsername())
                                                .build()
                                )
                                .build();

                        AppSyncClientFactory.getInstance(ShareTargetHandler.this)
                                .mutate(createHighlightMutation)
                                .enqueue(new GraphQLCall.Callback<CreateHighlightFromScreenshotMutation.Data>() {
                                    @Override
                                    public void onResponse(@Nonnull Response<CreateHighlightFromScreenshotMutation.Data> highlightResponse) {
                                        if (highlightResponse.hasErrors()) {
                                            // Error is handled in WebView.
                                            Log.e(Constants.LOG_TAG, highlightResponse.errors().toString());
                                            return;
                                        }

                                        final CreateHighlightFromScreenshotMutation.Data highlightData = highlightResponse.data();
                                        if (highlightData == null) {
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
                                                        CreateHighlightFromScreenshotMutation.CreateHighlightFromScreenshot highlight = highlightData.createHighlightFromScreenshot();
                                                        if (highlight != null) {
                                                            handleDisplayNotification(highlight.id());
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
                    public void onFailure(@Nonnull ApolloException e) {
                        // Error is handled in WebView.
                        Log.e(Constants.LOG_TAG, "ApolloException", e);
                    }
                });

    }

    private void handleSendNotSupported() {
        // TODO: implement fallback handling, e.g. display a "This does not look like a screenshot" UI
    }

    private void handleDisplayNotification(String noteId) {
        try {
            // NewHighlightFromShare may have deleted or changed the highlight.
            AppSyncClientFactory.getInstance(this).clearCaches(ClearCacheOptions.builder().clearQueries().build());
        } catch (ClearCacheException ex) {
            Log.e(Constants.LOG_TAG, "Unable to clear cache: ", ex);
        }

        AppSyncClientFactory.getInstance(this)
                .query(GetHighlightQuery.builder().id(noteId).build())
                .enqueue(new GraphQLCall.Callback<GetHighlightQuery.Data>() {
                    @Override
                    public void onResponse(@Nonnull Response<GetHighlightQuery.Data> response) {
                        if (response.hasErrors()) {
                            Log.e(Constants.LOG_TAG, response.errors().toString());
                            return;
                        }
                        GetHighlightQuery.Data data = response.data();
                        if (data == null) {
                            return;
                        }
                        GetHighlightQuery.GetHighlight highlight = data.getHighlight();
                        if (highlight == null) {
                            return;
                        }

                        Intent intent = new Intent(ShareTargetHandler.this, MainActivity.class);
                        intent.setData(Uri.parse(Constants.WEB_HOST + "/notes?id=" + highlight.id()));
                        PendingIntent pendingIntent = PendingIntent.getActivity(ShareTargetHandler.this, 0, intent, 0);

                        NotificationCompat.Builder builder = new NotificationCompat.Builder(ShareTargetHandler.this, Constants.NOTIFICATION_CHANNEL_NOTE_CREATED_ID)
                                .setSmallIcon(R.drawable.ic_stat_name)
                                .setColor(Color.BLACK)
                                .setContentTitle(Constants.NOTIFICATION_NOTE_CREATED_TITLE)
                                .setStyle(
                                        new NotificationCompat.BigTextStyle().bigText(highlight.text())
                                )
                                .setContentText(highlight.text())
                                .setPriority(NotificationCompat.PRIORITY_LOW)
                                .setContentIntent(pendingIntent)
                                .setAutoCancel(true);
                        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(ShareTargetHandler.this);
                        notificationManager.notify(highlight.id().hashCode(), builder.build());
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
