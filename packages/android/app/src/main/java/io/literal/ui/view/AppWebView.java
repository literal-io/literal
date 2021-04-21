package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.webkit.WebSettings;
import android.webkit.WebViewClient;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.webkit.WebMessageCompat;
import androidx.webkit.WebMessagePortCompat;
import androidx.webkit.WebViewCompat;
import androidx.webkit.WebViewFeature;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayDeque;
import java.util.UUID;

import io.literal.BuildConfig;
import io.literal.lib.Constants;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.repository.AnalyticsRepository;
import io.literal.repository.ErrorRepository;

public class AppWebView extends NestedScrollingChildWebView {

    private WebEvent.Callback webEventCallback;
    private WebViewClient externalWebViewClient;
    private ArrayDeque<String> baseHistory;

    public AppWebView(Context context) {
        super(context);
        setNestedScrollingEnabled(true);
    }

    public AppWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setNestedScrollingEnabled(true);
    }

    @SuppressLint("SetJavaScriptEnabled")
    public void initialize() {
        AppWebView.setWebContentsDebuggingEnabled(BuildConfig.DEBUG);
        this.addJavascriptInterface(new JavascriptInterface(), "literalWebview");
        WebSettings webSettings = this.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        this.setWebViewClient(this.webViewClient);
    }

    public void postWebEvent(WebEvent webEvent) {
        if (WebViewFeature.isFeatureSupported(WebViewFeature.POST_WEB_MESSAGE)) {
            WebViewCompat.postWebMessage(
                    this,
                    new WebMessageCompat(webEvent
                            .toJSON()
                            .toString()
                    ),
                    Uri.parse(WebRoutes.getWebHost())
            );

            try {
                JSONObject properties = new JSONObject();
                properties.put("target", "AppWebView");
                properties.put("event", webEvent.toJSON());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_DISPATCHED_WEB_EVENT, properties);
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            }
        }
    }

    public void onWebEvent(WebEvent.Callback cb) {
        this.webEventCallback = cb;
    }

    public void setExternalWebViewClient(WebViewClient externalWebViewClient) {
        this.externalWebViewClient = externalWebViewClient;
    }

    private void initializeWebMessageChannel() {
        if (WebViewFeature.isFeatureSupported(WebViewFeature.CREATE_WEB_MESSAGE_CHANNEL) &&
                WebViewFeature.isFeatureSupported(WebViewFeature.WEB_MESSAGE_PORT_SET_MESSAGE_CALLBACK) &&
                WebViewFeature.isFeatureSupported(WebViewFeature.POST_WEB_MESSAGE)) {
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
                            webEventCallback.onWebEvent(AppWebView.this, webEvent);
                        }

                        JSONObject loggedEvent = webEvent.toJSON(!webEvent.getType().startsWith("AUTH"));
                        Log.i("Literal", "Received WebEvent " + loggedEvent.toString());
                        if (!webEvent.getType().equals(WebEvent.TYPE_ANALYTICS_LOG_EVENT) && !webEvent.getType().equals(WebEvent.TYPE_ANALYTICS_SET_USER_ID)) {
                            JSONObject properties = new JSONObject();
                            properties.put("target", "AppWebView");
                            properties.put("event", loggedEvent);
                            AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_RECEIVED_WEB_EVENT, properties);
                        }
                    } catch (JSONException ex) {
                        Log.e("Literal", "Error in onMessage", ex);
                    }
                }
            });

            // Initial handshake - deliver WebMessageChannel port to JS.
            WebViewCompat.postWebMessage(
                    this,
                    new WebMessageCompat("", new WebMessagePortCompat[]{channel[1]}),
                    Uri.parse(WebRoutes.getWebHost())
            );
        }
    }

    public boolean handleBackPressed() {
        if (this.canGoBack()) {
            this.goBack();
            return true;
        }

        if (baseHistory != null && baseHistory.peek() != null) {
            String prev = baseHistory.poll();
            try {
                if (WebViewFeature.isFeatureSupported(WebViewFeature.POST_WEB_MESSAGE)) {
                    JSONObject data = new JSONObject();
                    data.put("url", prev);
                    WebViewCompat.postWebMessage(
                            this,
                            new WebMessageCompat(
                                    new WebEvent(WebEvent.TYPE_ROUTER_REPLACE, UUID.randomUUID().toString(), data)
                                            .toJSON()
                                            .toString()
                            ),
                            Uri.parse(WebRoutes.getWebHost())
                    );
                } else {
                    this.loadUrl(prev);
                }
            } catch (JSONException ex) {
                this.loadUrl(prev);
            }
            return true;
        }

        return false;
    }


    private class JavascriptInterface {
        @android.webkit.JavascriptInterface
        public boolean isWebview() {
            return true;
        }
    }

    private final WebViewClient webViewClient = new WebViewClient() {
        @Override
        public void onPageFinished(android.webkit.WebView webview, String url) {
            initializeWebMessageChannel();
            if (AppWebView.this.externalWebViewClient != null) {
                AppWebView.this.externalWebViewClient.onPageFinished(webview, url);
            }
        }

        @Override
        public void onPageStarted(android.webkit.WebView webview, String url, Bitmap favicon) {
            if (AppWebView.this.externalWebViewClient != null) {
                AppWebView.this.externalWebViewClient.onPageStarted(webview, url, favicon);
            }
        }
    };
}
