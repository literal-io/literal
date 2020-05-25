package io.literal.ui.activity;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Constants;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import io.literal.ui.view.WebView;

public class MainActivity extends AppCompatActivity {

    private WebView webView;

    private static final String WEB_DEFAULT_URL = Constants.WEB_HOST + "/notes";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        AWSMobileClientFactory.initializeClient(this);
        this.webView = findViewById(R.id.webview);
        this.webView.initialize(this);
        this.webView.requestFocus();

        Log.d(Constants.LOG_TAG, "MainActivity onCreate: " + (savedInstanceState == null ? "null" : savedInstanceState.toString()));

        if (savedInstanceState == null) {
            Intent intent = getIntent();
            if (intent != null) {
                Uri uri = intent.getData();
                if (uri != null) {
                    webView.loadUrlWithHistory(uri.toString(), new String[]{"/notes"});
                } else {
                    webView.loadUrl(WEB_DEFAULT_URL);
                }
            } else {
                webView.loadUrl(WEB_DEFAULT_URL);
            }
        }
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
}
