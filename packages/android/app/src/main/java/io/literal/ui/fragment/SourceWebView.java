package io.literal.ui.fragment;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;

import io.literal.R;
import io.literal.lib.Callback2;
import io.literal.viewmodel.SourceWebViewViewModel;

public class SourceWebView extends Fragment {

    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";

    private String paramInitialUrl;
    private io.literal.ui.view.SourceWebView webView;
    private SourceWebViewViewModel sourceWebViewViewModel;

    public SourceWebView() {
    }

    public static SourceWebView newInstance(String initialUrl) {
        SourceWebView fragment = new SourceWebView();
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
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_source_web_view, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        sourceWebViewViewModel = new ViewModelProvider(requireActivity()).get(SourceWebViewViewModel.class);

        webView = view.findViewById(R.id.source_web_view);
        webView.setOnPageFinished((e, webView, url) -> {
            if (!sourceWebViewViewModel.getHasFinishedInitializing().getValue()) {
                sourceWebViewViewModel.setHasFinishedInitializing(true);
            }
        });
        webView.setOnAnnotationCreated((e, _view) -> {
            String script = sourceWebViewViewModel.getInjectedGetSelectionScript(getActivity().getAssets()).getValue();
            webView.evaluateJavascript(script, new ValueCallback<String>() {
                @Override
                public void onReceiveValue(String value) {
                    sourceWebViewViewModel.createSelector(value);
                }
            });
        });

        if (savedInstanceState != null) {
            webView.restoreState(savedInstanceState);
        } else {
            webView.loadUrl(paramInitialUrl);
        }
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        if (this.webView != null) {
            webView.saveState(outState);
        }
    }
}