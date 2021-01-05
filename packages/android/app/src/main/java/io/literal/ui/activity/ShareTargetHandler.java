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
import android.util.Patterns;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebViewClient;
import android.widget.RelativeLayout;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
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
import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Error;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import java.io.File;
import java.util.List;
import java.util.UUID;
import java.util.function.Consumer;

import javax.annotation.Nonnull;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.AnnotationLib;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.FileActivityResultCallback;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.service.ShareTargetHandlerGraphQLService;
import io.literal.ui.view.SourceWebView;
import io.literal.ui.view.WebView;

public class ShareTargetHandler extends AppCompatActivity {

    private android.webkit.WebView webView;
    private ViewGroup splash;
    private ActivityResultLauncher<String> getFileContent;
    private final FileActivityResultCallback fileActivityResultCallback = new FileActivityResultCallback();

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_target_handler);

        splash = findViewById(R.id.splash);
        getFileContent = registerForActivityResult(new ActivityResultContracts.GetContent(), fileActivityResultCallback);

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

    private android.webkit.WebView initializeWebView() {
        WebView webView = new WebView(this);

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.share_target_handler_layout);
        RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
        webView.setLayoutParams(layoutParams);
        webView.setVisibility(View.INVISIBLE);
        layout.addView(webView);

        webView.initialize(ShareTargetHandler.this);
        webView.requestFocus();
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

        webView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onShowFileChooser(android.webkit.WebView webView, ValueCallback<Uri[]> filePathCallback, FileChooserParams fileChooserParams) {
                fileActivityResultCallback.setFilePathCallback(new ValueCallback<Uri[]>() {
                    @Override
                    public void onReceiveValue(Uri[] value) {
                        Uri[] absoluteUrls = new Uri[value.length];
                        for (int idx = 0; idx < value.length; idx++) {
                            File file = ContentResolverLib.toFile(ShareTargetHandler.this, value[idx], UUID.randomUUID().toString());
                            absoluteUrls[idx] = Uri.fromFile(file);
                        }
                        filePathCallback.onReceiveValue(absoluteUrls);
                    }
                });
                getFileContent.launch("image/*");
                return true;
            }
        });

        return webView;
    }

    private android.webkit.WebView initializeSourceWebView() {
        SourceWebView webView = new SourceWebView(this);

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.share_target_handler_layout);
        RelativeLayout.LayoutParams layoutParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
        layoutParams.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
        webView.setLayoutParams(layoutParams);
        webView.setVisibility(View.INVISIBLE);
        layout.addView(webView);

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(android.webkit.WebView view, String url) {
                view.setVisibility(View.VISIBLE);
                layout.removeView(splash);
                super.onPageFinished(view, url);
            }
        });

        return webView;
    }

    private void handleSignedIn(Bundle savedInstanceState) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel chan = new NotificationChannel(
                    Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_ID,
                    Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_NAME,
                    NotificationManager.IMPORTANCE_LOW
            );
            chan.setDescription(Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_DESCRIPTION);
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
                this.webView = this.initializeWebView();
                handleSendImage(intent);
            } else if (Intent.ACTION_SEND.equals(action) && type != null && type.equals("text/plain")) {
                String text = intent.getStringExtra(Intent.EXTRA_TEXT);
                if (Patterns.WEB_URL.matcher(text).matches()) {
                    this.webView = this.initializeSourceWebView();
                    handleAnnotateSource(intent);
                } else {
                    this.webView = this.initializeWebView();
                    handleSendText(intent);
                }
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

    private void handleAnnotateSource(Intent intent) {
        String sourceUrl = intent.getStringExtra(Intent.EXTRA_TEXT);
        webView.loadUrl(sourceUrl);
    }

    private void handleSendText(Intent intent) {
        String text = intent.getStringExtra(Intent.EXTRA_TEXT);
        ShareTargetHandlerGraphQLService.createAnnotationFromText(text, this, new ShareTargetHandlerGraphQLService.CreateListener<CreateAnnotationMutation.Data>() {
            @Override
            public void onAnnotationUri(String uri) {
                webView.loadUrl(uri);
            }

            @Override
            public void onAnnotationCreated(CreateAnnotationMutation.Data data) {
                if (data == null) {
                    // Error is handled in WebView
                    Log.i(Constants.LOG_TAG, "highlightResult is null");
                    return;
                }

                ((WebView) webView).onWebEvent(new WebEvent.Callback(ShareTargetHandler.this, (WebView) webView) {
                    @Override
                    public void onWebEvent(WebEvent event) {
                        super.onWebEvent(event);
                        switch (event.getType()) {
                            case WebEvent.TYPE_ACTIVITY_FINISH:
                                CreateAnnotationMutation.CreateAnnotation annotation = data.createAnnotation();
                                if (annotation != null) {
                                    handleDisplayNotification(annotation.annotation().id());
                                }
                                finish();
                        }
                    }
                });
            }

            @Override
            public void onError(Exception e) {
                Log.d("ShareTargetHandlerGraphQLService", "onError", e);
            }

            @Override
            public void onGraphQLError(List<Error> errors) {
                errors.forEach(new Consumer<Error>() {
                    @Override
                    public void accept(Error error) {
                        Log.d("ShareTargetHandlerGraphQLService", "onGraphQLError - " + error.message());
                    }
                });
            }
        });
    }

    private void handleSendImage(Intent intent) {
        Uri imageUri = intent.getParcelableExtra(Intent.EXTRA_STREAM);

        ShareTargetHandlerGraphQLService.createAnnotationFromImage(imageUri, this, new ShareTargetHandlerGraphQLService.CreateListener<CreateAnnotationFromExternalTargetMutation.Data>() {
            @Override
            public void onAnnotationUri(String uri) {
                webView.loadUrl(uri);
            }

            @Override
            public void onAnnotationCreated(CreateAnnotationFromExternalTargetMutation.Data data) {
                if (data == null) {
                    // Error is handled in WebView.CreateAnnotationMutation.Data
                    Log.i("Literal", "highlightResult is null");
                    return;
                }
                ((WebView) webView).onWebEvent(new WebEvent.Callback(ShareTargetHandler.this, (WebView) webView) {
                    @Override
                    public void onWebEvent(WebEvent event) {
                        super.onWebEvent(event);
                        switch (event.getType()) {
                            case WebEvent.TYPE_ACTIVITY_FINISH:
                                CreateAnnotationFromExternalTargetMutation.CreateAnnotationFromExternalTarget annotation = data.createAnnotationFromExternalTarget();
                                if (annotation != null) {
                                    handleDisplayNotification(annotation.id());
                                }
                                finish();
                        }
                    }
                });
            }

            @Override
            public void onError(Exception e) {
                Log.d("ShareTargetHandlerGraphQLService", "onError", e);
            }

            @Override
            public void onGraphQLError(List<Error> errors) {
                errors.forEach(new Consumer<Error>() {
                    @Override
                    public void accept(Error error) {
                        Log.d("ShareTargetHandlerGraphQLService", "onGraphQLError - " + error.message());
                    }
                });
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

        // FIXME: move to graphql service
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
                        Uri annotationUri = Uri.parse(
                                WebRoutes.creatorsIdAnnotationCollectionIdAnnotationId(
                                        AWSMobileClient.getInstance().getUsername(),
                                        Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT,
                                        AnnotationLib.idComponentFromId(annotation.id())
                                )
                        );
                        intent.setData(annotationUri);
                        PendingIntent pendingIntent = PendingIntent.getActivity(ShareTargetHandler.this, 0, intent, 0);

                        String textualTargetValue = null;
                        for (GetAnnotationForNotificationQuery.Target target : annotation.target()) {
                            GetAnnotationForNotificationQuery.AsTextualTarget textualTarget = target.asTextualTarget();
                            if (textualTarget != null) {
                                textualTargetValue = textualTarget.value();
                                break;
                            }
                        }

                        if (textualTargetValue != null) {
                            NotificationCompat.Builder builder = new NotificationCompat.Builder(ShareTargetHandler.this, Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_ID)
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
