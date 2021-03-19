package io.literal.ui.fragment;

import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.webkit.WebViewClient;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.amazonaws.mobile.client.results.Tokens;

import org.json.JSONObject;

import java.io.File;
import java.util.Map;
import java.util.UUID;

import io.literal.R;
import io.literal.lib.ContentResolverLib;
import io.literal.lib.FileActivityResultCallback;
import io.literal.lib.WebEvent;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;

public class AppWebView extends Fragment {
    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";
    private static final String PARAM_APP_WEB_VIEW_MODEL_KEY = "PARAM_APP_WEB_VIEW_MODEL_KEY";

    private final FileActivityResultCallback fileActivityResultCallback = new FileActivityResultCallback();

    private String paramInitialUrl;
    private String paramAppWebViewModelKey;

    private ActivityResultLauncher<String> getFileContent;
    private AppWebViewViewModel appWebViewViewModel;
    private AuthenticationViewModel authenticationViewModel;
    private io.literal.ui.view.AppWebView appWebView;
    private final WebEvent.Callback webEventCallback = new WebEvent.Callback() {

        private void handleSignIn(io.literal.ui.view.AppWebView view) {
            authenticationViewModel.signInGoogle(getActivity(), (e, _void) -> {
                if (e != null) {
                    Log.d("AppWebView", "handleSignIn", e);
                    return;
                }
                Tokens tokens = authenticationViewModel.getTokens().getValue();
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

        private void handleGetTokens(io.literal.ui.view.AppWebView view) {
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

        private void handleGetUserInfo(io.literal.ui.view.AppWebView view) {
            try {
                JSONObject result = new JSONObject();
                result.put("username", authenticationViewModel.getUsername().getValue());
                result.put("id", authenticationViewModel.getIdentityId().getValue());
                Map<String, String> userAttributes = authenticationViewModel.getUserAttributes().getValue();
                result.put("attributes", userAttributes != null ? new JSONObject(userAttributes) : null);
                appWebView.postWebEvent(
                        new WebEvent(WebEvent.TYPE_AUTH_GET_USER_INFO_RESULT, UUID.randomUUID().toString(), result)
                );
            } catch (Exception e) {
                Log.d("AppWebView", "handleGetUserInfo", e);
            }
        }

        public void onWebEvent(io.literal.ui.view.AppWebView view, WebEvent event) {
            appWebViewViewModel.dispatchReceivedWebEvent(event);
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

    public AppWebView() {
    }

    public static AppWebView newInstance(String initialUrl, String appWebViewModelKey) {
        AppWebView fragment = new AppWebView();
        Bundle args = new Bundle();
        args.putString(PARAM_INITIAL_URL, initialUrl);
        args.putString(PARAM_APP_WEB_VIEW_MODEL_KEY, appWebViewModelKey);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            paramInitialUrl = getArguments().getString(PARAM_INITIAL_URL);
            paramAppWebViewModelKey = getArguments().getString(PARAM_APP_WEB_VIEW_MODEL_KEY);
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

        appWebViewViewModel =
                paramAppWebViewModelKey != null
                        ? new ViewModelProvider(requireActivity()).get(paramAppWebViewModelKey, AppWebViewViewModel.class)
                        : new ViewModelProvider(requireActivity()).get(AppWebViewViewModel.class);
        authenticationViewModel = new ViewModelProvider(requireActivity()).get(AuthenticationViewModel.class);

        appWebView = view.findViewById(R.id.app_web_view);

        appWebView.initialize();
        appWebView.setExternalWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(android.webkit.WebView view, String url) {
                if (!appWebViewViewModel.getHasFinishedInitializing().getValue()) {
                    appWebViewViewModel.setHasFinishedInitializing(true);
                }
            }

            @Override
            public void onPageStarted(android.webkit.WebView view, String url, Bitmap favicon) {
                String viewModelUrl = appWebViewViewModel.getUrl().getValue();
                if (viewModelUrl == null || !viewModelUrl.equals(url)) {
                    appWebViewViewModel.setUrl(url);
                }
            }
        });
        appWebView.setWebChromeClient(new WebChromeClient() {
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
        appWebView.onWebEvent(webEventCallback);
        appWebViewViewModel.getUrl().observe(requireActivity(), (url) -> {
            String currentUrl = appWebView.getUrl();
            if (currentUrl == null || (url != null && !appWebView.getUrl().equals(url))) {
                appWebView.loadUrl(url);
            }
        });

        appWebViewViewModel.getWebEvents().observe(requireActivity(), (webEvents) -> {
            if (webEvents == null) {
                return;
            }

            webEvents.iterator().forEachRemaining((webEvent) -> {
                appWebView.postWebEvent(webEvent);
            });

            appWebViewViewModel.clearWebEvents();
        });

        if (savedInstanceState != null) {
            appWebView.restoreState(savedInstanceState);
        } else {
            appWebViewViewModel.setUrl(paramInitialUrl);
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (this.appWebView != null) {
            appWebView.saveState(outState);
        }
    }


    public io.literal.ui.view.AppWebView getWebView() { return this.appWebView; }
    public void postWebEvent(WebEvent webEvent) {
        this.appWebView.postWebEvent(webEvent);
    }
}