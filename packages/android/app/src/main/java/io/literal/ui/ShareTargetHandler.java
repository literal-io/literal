package io.literal.ui;

import io.literal.BuildConfig;
import io.literal.R;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.ContentResolverLib;

import type.CreateHighlightFromScreenshotInput;
import type.CreateScreenshotInput;
import type.S3ObjectInput;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.webkit.WebSettings;
import android.webkit.WebView;

import com.amazonaws.amplify.generated.graphql.CreateHighlightFromScreenshotMutation;
import com.amazonaws.amplify.generated.graphql.CreateScreenshotMutation;

import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONObject;

import java.io.File;
import java.util.UUID;
import java.util.concurrent.CountDownLatch;

import javax.annotation.Nonnull;

public class ShareTargetHandler extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_target_handler);

        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();

        if (Intent.ACTION_SEND.equals(action) && type != null && type.startsWith("image/")) {
            handleSendImage(intent);
        } else {
            handleSendNotSupported();
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
                .enqueue(createHighlightCallback.getCallback());

        // TODO: handle errors causing screenshot to not be created.
        CreateHighlightFromScreenshotMutation.Data highlightResult = null;
        try {
            highlightResult = createHighlightCallback.awaitResult();
        } catch (InterruptedException e) {}
        if (highlightResult == null) {
            Log.i("Literal", "highlightResult is null");
            return;
        }
        Log.i("Literal", "highlight created.");
        this.initializeWebView("https://literal.io/notes/" + highlightId);
    }

    void handleSendNotSupported() {
        // TODO: implement fallback handling, e.g. display a "This does not look like a screenshot" UI
    }

    void initializeWebView(String initialURL) {
        WebView webView = (WebView) findViewById(R.id.webview);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            webView.setWebContentsDebuggingEnabled(BuildConfig.DEBUG);
        }
        WebSettings webSettings = webView.getSettings();
        webSettings.setJavaScriptEnabled(true);

        webView.loadUrl(initialURL);
    }

    private LatchedGraphQLCallback<CreateScreenshotMutation.Data> createScreenshotCallback = new LatchedGraphQLCallback<>();
    private LatchedGraphQLCallback<CreateHighlightFromScreenshotMutation.Data> createHighlightCallback = new LatchedGraphQLCallback<>();

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
