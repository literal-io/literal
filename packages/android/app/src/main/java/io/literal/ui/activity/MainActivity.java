package io.literal.ui.activity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.app.AppCompatActivity;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.WebRoutes;
import io.literal.ui.view.WebView;

public class MainActivity extends AppCompatActivity {

    private WebView webView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.webView = findViewById(R.id.webview);
        ViewGroup splash = findViewById(R.id.splash);
        ViewGroup layout = (ViewGroup) splash.getParent();

        this.webView.setVisibility(View.INVISIBLE);
        this.webView.onPageFinished(new WebView.PageFinishedCallback() {
            @Override
            public void onPageFinished(android.webkit.WebView view, String Url) {
                view.setVisibility(View.VISIBLE);
                layout.removeView(splash);
            }
        });

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
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        handleSignedOut();
                    }
                });
            }
        });
    }

    private void handleSignedIn(Bundle savedInstanceState) {
        webView.initialize(this);
        webView.requestFocus();

        String username = AWSMobileClient.getInstance().getUsername();
        String defaultUrl =
                username != null
                        ? WebRoutes.creatorsIdAnnotationCollectionId(username, Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT)
                        : WebRoutes.authenticate();

        if (savedInstanceState == null) {
            Intent intent = getIntent();
            if (intent != null) {
                Uri uri = intent.getData();
                if (uri != null) {
                    webView.loadUrlWithHistory(uri.toString(), new String[]{"/annotations"});
                } else {
                    webView.loadUrl(defaultUrl);
                }
            } else {
                webView.loadUrl(defaultUrl);
            }
        }
    }

    private void handleSignedOut() {
        webView.initialize(this);
        webView.requestFocus();
        webView.loadUrl(WebRoutes.authenticate());
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
