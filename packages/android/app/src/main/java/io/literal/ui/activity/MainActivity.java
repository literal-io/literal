package io.literal.ui.activity;

import io.literal.R;
import io.literal.lib.Constants;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.webkit.WebView;

public class MainActivity extends AppCompatActivity {

    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.webView = findViewById(R.id.webview);

        if (savedInstanceState == null) {
            webView.loadUrl(Constants.WEB_HOST + "/notes");
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
