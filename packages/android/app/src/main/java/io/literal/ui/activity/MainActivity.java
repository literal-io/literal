package io.literal.ui.activity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.UserStateListener;
import com.amazonaws.mobileconnectors.cognitoauth.AuthClient;

import java.io.File;
import java.util.UUID;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.FileActivityResultCallback;
import io.literal.lib.WebRoutes;
import io.literal.ui.view.WebView;

public class MainActivity extends AppCompatActivity {

    private WebView webView;
    private ActivityResultLauncher<String> getFileContent;

    private FileActivityResultCallback fileActivityResultCallback = new FileActivityResultCallback();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getFileContent = registerForActivityResult(new ActivityResultContracts.GetContent(), fileActivityResultCallback);

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

        this.webView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onShowFileChooser(android.webkit.WebView webView, ValueCallback<Uri[]> filePathCallback, FileChooserParams fileChooserParams) {
                fileActivityResultCallback.setFilePathCallback(new ValueCallback<Uri[]>() {
                    @Override
                    public void onReceiveValue(Uri[] value) {
                        Uri[] absoluteUrls = new Uri[value.length];
                        for (int idx = 0; idx < value.length; idx++) {
                            File file = ContentResolverLib.toFile(MainActivity.this, value[idx], UUID.randomUUID().toString());
                            absoluteUrls[idx] = Uri.fromFile(file);
                        }
                        filePathCallback.onReceiveValue(absoluteUrls);
                    }
                });
                getFileContent.launch("image/*");
                return true;
            }
        });

        AWSMobileClientFactory.initializeClient(this, new Callback<UserStateDetails>() {
            @Override
            public void onResult(UserStateDetails result) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        switch (result.getUserState()) {
                            case SIGNED_IN:
                                handleSignedIn(savedInstanceState);
                                break;
                            default:
                                handleSignedOut();
                        }
                    }
                });
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
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(Constants.LOG_TAG, "onActivityResult");
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == AuthClient.CUSTOM_TABS_ACTIVITY_CODE) {
            AWSMobileClient.getInstance().handleAuthResponse(data);
        }
    }
}
