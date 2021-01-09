package io.literal.ui.fragment;

import android.content.Intent;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProvider;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.HostedUIOptions;
import com.amazonaws.mobile.client.SignInUIOptions;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.results.Tokens;

import org.json.JSONObject;

import java.io.File;
import java.util.Map;
import java.util.UUID;

import io.literal.R;
import io.literal.lib.Constants;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.FileActivityResultCallback;
import io.literal.lib.WebEvent;
import io.literal.repository.AuthenticationRepository;
import io.literal.ui.activity.ShareTargetHandler;
import io.literal.ui.view.WebView;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;

public class AppWebView extends Fragment {
    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";
    private final FileActivityResultCallback fileActivityResultCallback = new FileActivityResultCallback();

    private String paramInitialUrl;
    private ActivityResultLauncher<String> getFileContent;
    private AppWebViewViewModel appWebViewViewModel;
    private AuthenticationViewModel authenticationViewModel;
    private WebView webView;

    public AppWebView() {
    }

    public static AppWebView newInstance(String initialUrl) {
        AppWebView fragment = new AppWebView();
        Bundle args = new Bundle();
        args.putString(PARAM_INITIAL_URL, initialUrl);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            paramInitialUrl = getArguments().getString(PARAM_INITIAL_URL);
        }
        getFileContent = registerForActivityResult(new ActivityResultContracts.GetContent(), fileActivityResultCallback);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_app_web_view, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        appWebViewViewModel = new ViewModelProvider(requireActivity()).get(AppWebViewViewModel.class);
        authenticationViewModel = new ViewModelProvider(requireActivity()).get(AuthenticationViewModel.class);

        webView = view.findViewById(R.id.app_web_view);

        webView.initialize();
        webView.onPageFinished(new WebView.PageFinishedCallback() {
            @Override
            public void onPageFinished(android.webkit.WebView view, String url) {
                if (!appWebViewViewModel.getHasFinishedInitializing().getValue()) {
                    appWebViewViewModel.setHasFinishedInitializing(true);
                }
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
                            File file = ContentResolverLib.toFile(getActivity(), value[idx], UUID.randomUUID().toString());
                            absoluteUrls[idx] = Uri.fromFile(file);
                        }
                        filePathCallback.onReceiveValue(absoluteUrls);
                    }
                });
                getFileContent.launch("image/*");
                return true;
            }
        });
        webView.onWebEvent(webEventCallback);

        if (savedInstanceState != null) {
            webView.restoreState(savedInstanceState);
        } else {
            webView.loadUrl(paramInitialUrl);
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (this.webView != null) {
            webView.saveState(outState);
        }
    }

    private final WebEvent.Callback webEventCallback = new WebEvent.Callback() {

        private void handleSignIn(WebView view) {
            authenticationViewModel.signInGoogle(getActivity(), (e, userStateDetails) -> {

                Log.d("AppWebView", "handleSignIn callback");
                if (e != null) {
                    Log.d("AppWebView", "handleSignIn", e);
                    return;
                }

                Tokens tokens = authenticationViewModel.getTokens().getValue();
                Log.d("AppWebView", "handleSignIn tokens: " + tokens);
                JSONObject result = new JSONObject();
                try {
                    result.put("idToken", tokens.getIdToken().getTokenString());
                    result.put("refreshToken", tokens.getRefreshToken().getTokenString());
                    result.put("accessToken", tokens.getAccessToken().getTokenString());

                    getActivity().runOnUiThread(() -> {
                        view.postWebEvent(
                                new WebEvent(WebEvent.TYPE_AUTH_SIGN_IN_RESULT, UUID.randomUUID().toString(), result)
                        );
                    });
                } catch (Exception jsonException) {
                    Log.d("AppWebView", "handleSignIn", jsonException);
                }
            });
        }

        private void handleGetTokens(WebView view) {
            Tokens tokens = authenticationViewModel.getTokens().getValue();

            JSONObject result = new JSONObject();
            try {
                result.put("idToken", tokens.getIdToken().getTokenString());
                result.put("refreshToken", tokens.getRefreshToken().getTokenString());
                result.put("accessToken", tokens.getAccessToken().getTokenString());

                view.postWebEvent(
                        new WebEvent(WebEvent.TYPE_AUTH_GET_TOKENS_RESULT, UUID.randomUUID().toString(), result)
                );
            } catch (Exception e) {
                Log.d("AppWebView", "handleGetTokens", e);
            }
        }

        private void handleGetUserInfo(WebView view) {

            try {
                JSONObject result = new JSONObject();
                result.put("username", authenticationViewModel.getUsername().getValue());
                result.put("attributes", new JSONObject(authenticationViewModel.getUserAttributes().getValue()));
                result.put("id", authenticationViewModel.getIdentityId().getValue());

                webView.postWebEvent(
                        new WebEvent(WebEvent.TYPE_AUTH_GET_USER_INFO_RESULT, UUID.randomUUID().toString(), result)
                );
            } catch (Exception e) {
                Log.d("AppWebView", "handleGetUserInfo", e);
            }
        }

        public void onWebEvent(WebView view, WebEvent event) {
            switch (event.getType()) {
                case WebEvent.TYPE_AUTH_SIGN_IN:
                    this.handleSignIn(view);
                    return;
                case WebEvent.TYPE_AUTH_GET_TOKENS:
                    this.handleGetTokens(view);
                    return;
                case WebEvent.TYPE_AUTH_GET_USER_INFO:
                    this.handleGetUserInfo(view);
                    return;
            }
        }
    };
}