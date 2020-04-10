package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.webkit.WebSettings;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.webkit.WebMessageCompat;
import androidx.webkit.WebMessagePortCompat;
import androidx.webkit.WebViewCompat;
import androidx.webkit.WebViewFeature;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.BuildConfig;
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
    }

    public void onWebEvent(WebEventCallback cb) {
        this.webEventCallback = cb;
    }

    // TODO: call when webview content is loaded
    private void initializeWebMessageChannel() {
        if (WebViewFeature.isFeatureSupported(WebViewFeature.CREATE_WEB_MESSAGE_CHANNEL) &&
                WebViewFeature.isFeatureSupported(WebViewFeature.WEB_MESSAGE_PORT_SET_MESSAGE_CALLBACK)) {
            final WebMessagePortCompat[] channel = WebViewCompat.createWebMessageChannel(this);
            final WebMessagePortCompat port = channel[0];
            final Handler handler = new Handler(Looper.getMainLooper());
            port.setWebMessageCallback(handler, new WebMessagePortCompat.WebMessageCallbackCompat() {
                @SuppressLint("RequiresFeature")
                @Override
                public void onMessage(@NonNull WebMessagePortCompat port, @Nullable WebMessageCompat message) {
                    super.onMessage(port, message);
                    String data = message.getData();
                    try {
                        JSONObject json = new JSONObject(data);
                        WebEvent webEvent = new WebEvent(json);
                        if (webEventCallback != null) {
                            webEventCallback.onWebEvent(webEvent);
                        }
                    } catch (JSONException ex) {
                        Log.e("Literal", "Error in onMessage", ex);
                    }
                }
            });
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
