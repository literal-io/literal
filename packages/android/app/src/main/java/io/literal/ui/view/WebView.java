package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.net.Uri;
import android.os.Build;
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

import io.literal.BuildConfig;
import io.literal.lib.Constants;
import io.literal.lib.WebEvent;

public class WebView extends android.webkit.WebView {

    private WebEventCallback webEventCallback;

    public WebView(Context context) {
        super(context);
        this.initialize();
    }

    public WebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.initialize();
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void initialize() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            WebView.setWebContentsDebuggingEnabled(BuildConfig.DEBUG);
        }
        WebSettings webSettings = this.getSettings();
        webSettings.setJavaScriptEnabled(true);

        this.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(android.webkit.WebView webview, String url) {
                initializeWebMessageChannel();
            }
        });
    }

    public void onWebEvent(WebEventCallback cb) {
        this.webEventCallback = cb;
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
                        Log.i("Literal", "Received WebEvent " + json.toString());
                        WebEvent webEvent = new WebEvent(json);
                        if (webEventCallback != null) {
                            webEventCallback.onWebEvent(webEvent);
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
                    Uri.parse(Constants.WEB_HOST)
            );
        }
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK) && this.canGoBack()) {
            this.goBack();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    public interface WebEventCallback {
        void onWebEvent(WebEvent event);
    }
}
