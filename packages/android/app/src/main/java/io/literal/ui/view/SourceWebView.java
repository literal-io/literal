package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.webkit.WebChromeClient;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.webkit.WebMessageCompat;
import androidx.webkit.WebMessagePortCompat;
import androidx.webkit.WebViewCompat;
import androidx.webkit.WebViewFeature;

import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.ObjectMetadata;
import com.amazonaws.services.s3.model.S3Object;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import io.literal.BuildConfig;
import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Callback;
import io.literal.lib.Callback2;
import io.literal.lib.ResultCallback;
import io.literal.lib.WebEvent;
import io.literal.repository.AnalyticsRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.StorageRepository;

public class SourceWebView extends NestedScrollingChildWebView {

    private Callback<Exception, ActionMode> onAnnotationCreated;
    private Callback<Exception, ActionMode> onAnnotationCommitEdit;
    private Callback<Exception, ActionMode> onAnnotationCancelEdit;
    private Callback2<View, Bitmap> onReceivedIcon;

    private ResultCallback<String, Void> onGetWebMessageChannelInitializerScript;
    private ResultCallback<Integer, Void> onGetTextSelectionMenu;

    private Callback2<SourceWebView, WebEvent> webEventCallback;
    private WebViewClient externalWebViewClient;

    public SourceWebView(Context context) {
        this(context, null);
    }

    public SourceWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.initialize();
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void initialize() {
        SourceWebView.setWebContentsDebuggingEnabled(BuildConfig.DEBUG);
        WebSettings webSettings = this.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        webSettings.setBuiltInZoomControls(true);
        webSettings.setDisplayZoomControls(false);
        this.setWebViewClient(new SourceWebViewClient(getContext()));

        this.setWebChromeClient(new WebChromeClient() {
            @Override
            public void onReceivedIcon(WebView view, Bitmap icon) {
                if (SourceWebView.this.onReceivedIcon != null) {
                    onReceivedIcon.invoke(null, view, icon);
                }
                super.onReceivedIcon(view, icon);
            }
        });
    }

    public void postWebEvent(WebEvent webEvent) {
        if (WebViewFeature.isFeatureSupported(WebViewFeature.POST_WEB_MESSAGE)) {
            WebViewCompat.postWebMessage(
                    this,
                    new WebMessageCompat(webEvent
                            .toJSON()
                            .toString()
                    ),
                    Uri.parse("*")
            );
            try {
                JSONObject properties = new JSONObject();
                properties.put("target", "SourceWebView");
                properties.put("event", webEvent.toJSON());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_DISPATCHED_WEB_EVENT, properties);
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            }
        }
    }

    private void initializeWebMessageChannel() {
        if (WebViewFeature.isFeatureSupported(WebViewFeature.CREATE_WEB_MESSAGE_CHANNEL) &&
                WebViewFeature.isFeatureSupported(WebViewFeature.WEB_MESSAGE_PORT_SET_MESSAGE_CALLBACK) &&
                WebViewFeature.isFeatureSupported(WebViewFeature.POST_WEB_MESSAGE)) {

            // Ensure web context is setup to begin the message channel handshake.
            if (this.onGetWebMessageChannelInitializerScript != null) {
                String script = this.onGetWebMessageChannelInitializerScript.invoke(null, null);
                this.evaluateJavascript(script, (value -> { /** noop **/}));
            }

            final WebMessagePortCompat[] channel = WebViewCompat.createWebMessageChannel(this);
            final Handler handler = new Handler(Looper.getMainLooper());
            channel[0].setWebMessageCallback(handler, new WebMessagePortCompat.WebMessageCallbackCompat() {
                @SuppressLint("RequiresFeature")
                @Override
                public void onMessage(@NonNull WebMessagePortCompat port, @Nullable WebMessageCompat message) {
                    super.onMessage(port, message);
                    String data = message.getData();
                    try {
                        JSONObject json = new JSONObject(data);
                        WebEvent webEvent = new WebEvent(json);
                        if (webEventCallback != null) {
                            webEventCallback.invoke(null, SourceWebView.this, webEvent);
                        }

                        JSONObject properties = new JSONObject();
                        properties.put("target", "SourceWebView");
                        properties.put("event", json);
                        AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_RECEIVED_WEB_EVENT, properties);
                    } catch (JSONException ex) {
                        ErrorRepository.captureException(ex);
                    }
                }
            });

            // Initial handshake - deliver WebMessageChannel port to JS.
            WebViewCompat.postWebMessage(
                    this,
                    new WebMessageCompat("", new WebMessagePortCompat[]{channel[1]}),
                    Uri.parse("*")
            );
        }
    }

    @Override
    public ActionMode startActionMode(ActionMode.Callback callback) {
        if (onGetTextSelectionMenu != null) {
            ActionMode.Callback2 cb = new CreateAnnotationActionModeCallback(
                    (ActionMode.Callback2) callback,
                    onGetTextSelectionMenu.invoke(null, null)
            );
            return super.startActionMode(cb);
        }
        return super.startActionMode(callback);
    }

    @Override
    public ActionMode startActionMode(ActionMode.Callback callback, int type) {
        if (callback instanceof EditAnnotationActionModeCallback) {
            ActionMode actionMode = super.startActionMode(callback, type);
            return actionMode;
        } else if (onGetTextSelectionMenu != null) {
            // Default Chrome text selection action mode, which we intercept to provide different menu options.
            ActionMode.Callback2 cb = new CreateAnnotationActionModeCallback(
                    (ActionMode.Callback2) callback,
                    onGetTextSelectionMenu.invoke(null, null)
            );
            ActionMode actionMode = super.startActionMode(cb, type);
            return actionMode;
        }
        return super.startActionMode(callback, type);
    }

    public ActionMode startEditAnnotationActionMode(
            String getAnnotationBoundingBoxScript,
            Rect initialAnnotationBoundingBox,
            Callback<Void, Void> onEditAnnotation,
            Callback<Void, Void> onDeleteAnnotation
    ) {
        EditAnnotationActionModeCallback actionModeCallback = new EditAnnotationActionModeCallback(initialAnnotationBoundingBox, onEditAnnotation, onDeleteAnnotation);
        ActionMode actionMode = super.startActionMode(actionModeCallback, ActionMode.TYPE_FLOATING);

        setOnScrollChangeListener((v, scrollX, scrollY, oldScrollX, oldScrollY) -> evaluateJavascript(getAnnotationBoundingBoxScript, value -> {
            try {
                JSONObject boundingBox = new JSONObject(value);
                int left = boundingBox.getInt("left");
                int top = boundingBox.getInt("top");
                int right = boundingBox.getInt("right");
                int bottom = boundingBox.getInt("bottom");

                actionModeCallback.setAnnotationBoundingBox(new Rect(left, top, right, bottom));
                actionMode.invalidateContentRect();
            } catch (JSONException ex) {
                ErrorRepository.captureException(ex);
            }
        }));

        return actionMode;
    }

    public void finishEditAnnotationActionMode(ActionMode actionMode) {
        setOnScrollChangeListener(null);
        actionMode.finish();
    }

    public void setOnAnnotationCreated(Callback<Exception, ActionMode> onAnnotationCreated) {
        this.onAnnotationCreated = onAnnotationCreated;
    }

    public void setOnAnnotationCommitEdit(Callback<Exception, ActionMode> onAnnotationCommitEdit) {
        this.onAnnotationCommitEdit = onAnnotationCommitEdit;
    }

    public void setOnAnnotationCancelEdit(Callback<Exception, ActionMode> onAnnotationCancelEdit) {
        this.onAnnotationCancelEdit = onAnnotationCancelEdit;
    }

    public void setOnReceivedIcon(Callback2<View, Bitmap> onReceivedIcon) {
        this.onReceivedIcon = onReceivedIcon;
    }

    public void setExternalWebViewClient(WebViewClient externalWebViewClient) {
        this.externalWebViewClient = externalWebViewClient;
    }

    public void setOnGetWebMessageChannelInitializerScript(ResultCallback<String, Void> onGetWebMessageChannelInitializerScript) {
        this.onGetWebMessageChannelInitializerScript = onGetWebMessageChannelInitializerScript;
    }

    public void setWebEventCallback(Callback2<SourceWebView, WebEvent> webEventCallback) {
        this.webEventCallback = webEventCallback;
    }

    public void setOnGetTextSelectionMenu(ResultCallback<Integer, Void> onGetTextSelectionMenu) {
        this.onGetTextSelectionMenu = onGetTextSelectionMenu;
    }

    public boolean handleBackPressed() {
        if (this.canGoBack()) {
            this.goBack();
            return true;
        }

        return false;
    }

    public static class EditAnnotationActionModeCallback extends ActionMode.Callback2 {
        Rect annotationBoundingBox;
        Callback<Void, Void> onEditAnnotation;
        Callback<Void, Void> onDeleteAnnotation;

        public EditAnnotationActionModeCallback(@NotNull Rect annotationBoundingBox, @NotNull Callback<Void, Void> onEditAnnotation, @NotNull Callback<Void, Void> onDeleteAnnotation) {
            this.annotationBoundingBox = annotationBoundingBox;
            this.onEditAnnotation = onEditAnnotation;
            this.onDeleteAnnotation = onDeleteAnnotation;
        }

        @Override
        public boolean onCreateActionMode(ActionMode mode, Menu menu) {
            MenuInflater inflater = mode.getMenuInflater();
            inflater.inflate(R.menu.source_webview_edit_annotation_menu, menu);
            return true;
        }

        public void setAnnotationBoundingBox(Rect annotationBoundingBox) {
            this.annotationBoundingBox = annotationBoundingBox;
        }

        @Override
        public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
            return false;
        }

        @Override
        public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
            switch (item.getItemId()) {
                case R.id.menu_item_edit:
                    onEditAnnotation.invoke(null, null);
                    return true;
                case R.id.menu_item_remove:
                    onDeleteAnnotation.invoke(null, null);
                    return true;
                default:
                    return false;
            }
        }

        @Override
        public void onDestroyActionMode(ActionMode mode) {

        }

        @Override
        public void onGetContentRect(ActionMode mode, View view, Rect outRect) {
            outRect.set(annotationBoundingBox);
        }
    }

    private class CreateAnnotationActionModeCallback extends ActionMode.Callback2 {

        ActionMode.Callback2 originalCallback;
        int menu;

        public CreateAnnotationActionModeCallback(ActionMode.Callback2 originalCallback, int menu) {
            this.originalCallback = originalCallback;
            this.menu = menu;
        }

        @Override
        public boolean onCreateActionMode(ActionMode mode, Menu menu) {
            MenuInflater inflater = mode.getMenuInflater();
            inflater.inflate(this.menu, menu);
            return true;
        }

        @Override
        public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
            return false;
        }

        @Override
        public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
            switch (item.getItemId()) {
                case R.id.menu_item_annotate:
                    if (onAnnotationCreated != null) {
                        onAnnotationCreated.invoke(null, mode);
                    }

                    // FIXME: call this after getting the selection from the webview
                    mode.finish();
                    return true;
                case R.id.menu_item_commit_edit:
                    if (onAnnotationCommitEdit != null) {
                        onAnnotationCommitEdit.invoke(null, mode);
                    }
                    mode.finish();
                    return true;
                case R.id.menu_item_cancel_edit:
                    if (onAnnotationCancelEdit != null) {
                        onAnnotationCancelEdit.invoke(null, mode);
                    }
                    mode.finish();
                    return true;
                default:
                    return false;
            }
        }

        @Override
        public void onDestroyActionMode(ActionMode mode) {
            this.originalCallback.onDestroyActionMode(mode);
        }

        @Override
        public void onGetContentRect(ActionMode mode, View view, Rect outRect) {
            originalCallback.onGetContentRect(mode, view, outRect);
        }
    };

    private class SourceWebViewClient extends WebViewClient {

        private final String storageHost;
        private final String storageBucket;

        public SourceWebViewClient(Context context) {
            storageBucket = StorageRepository.getBucketName(context);
            storageHost = storageBucket + ".s3." + StorageRepository.getBucketRegion(context) + ".amazonaws.com";
        }

        @Nullable
        @Override
        public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
            if (request == null) {
                return null;
            }
            String host = request.getUrl().getHost();
            String path = request.getUrl().getPath().substring(1);
            if (!request.getMethod().equals("GET") || host == null || !host.equals(storageHost) || path == null) {
                return null;
            }

            try {
                S3Object object = AppSyncClientFactory.getS3Client(getContext()).getObject(storageBucket, request.getUrl().getPath().substring(1));
                ObjectMetadata metadata = object.getObjectMetadata();
                WebResourceResponse response = new WebResourceResponse(
                        metadata.getContentType(),
                        null,
                        object.getObjectContent()
                );

                Map<String, String> responseHeaders = new HashMap<>();
                responseHeaders.put("Cache-Control", metadata.getCacheControl());
                responseHeaders.put("ETag", metadata.getETag());
                response.setResponseHeaders(responseHeaders);
                return response;
            } catch (Exception ex) {
                ErrorRepository.captureException(ex, request.getUrl().getPath().substring(1));
                return null;
            }
        }

        @Override
        public void onPageFinished(android.webkit.WebView webview, String url) {
            initializeWebMessageChannel();
            if (SourceWebView.this.externalWebViewClient != null) {
                SourceWebView.this.externalWebViewClient.onPageFinished(webview, url);
            }
        }

        @Override
        public void onPageStarted(android.webkit.WebView webview, String url, Bitmap favicon) {
            if (SourceWebView.this.externalWebViewClient != null) {
                SourceWebView.this.externalWebViewClient.onPageStarted(webview, url, favicon);
            }
        }
    }
}
