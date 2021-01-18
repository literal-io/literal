package io.literal.ui.fragment;

import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;
import android.webkit.WebIconDatabase;

import org.json.JSONArray;
import org.json.JSONException;

import java.net.MalformedURLException;
import java.net.URL;

import io.literal.R;
import io.literal.viewmodel.SourceWebViewViewModel;

public class SourceWebView extends Fragment {

    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";

    private String paramInitialUrl;
    private io.literal.ui.view.SourceWebView webView;
    private Toolbar toolbar;
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

        toolbar = view.findViewById(R.id.toolbar);
        ((AppCompatActivity) getActivity()).setSupportActionBar(toolbar);

        webView = view.findViewById(R.id.source_web_view);
        webView.setOnReceivedIcon((e, webView, icon) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnReceivedIcon callback error:", e);
            }

            SourceWebViewViewModel.DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
            sourceWebViewViewModel.setDomainMetadata(domainMetadata != null ? domainMetadata.getUrl() : null, icon);
        });
        webView.setOnPageStarted((e, webView, url, bitmap) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnPageStarted callback error:", e);
                return;
            }

            SourceWebViewViewModel.DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
            try {
                sourceWebViewViewModel.setDomainMetadata(new URL(url), domainMetadata != null ? domainMetadata.getFavicon() : null);
            } catch (MalformedURLException ex) {
                Log.d("SourceWebView", "Unable to execute setDomainMetadata:", ex);
            }
        });
        webView.setOnPageFinished((e, webView, url) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnPageFinished callback error:", e);
                return;
            }

            if (!sourceWebViewViewModel.getHasFinishedInitializing().getValue()) {
                sourceWebViewViewModel.setHasFinishedInitializing(true);
            }
        });
        webView.setOnAnnotationCreated((e, _view) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnAnnotationCreated callback error:", e);
                return;
            }

            String script = sourceWebViewViewModel.getGetSelectorScript(getActivity().getAssets());
            webView.evaluateJavascript(script, new ValueCallback<String>() {
                @Override
                public void onReceiveValue(String value) {
                    sourceWebViewViewModel.createSelector(value);
                }
            });
        });

        sourceWebViewViewModel.getSelectors().observe(getActivity(), (selectors) -> {
            if (this.webView == null) {
                Log.d("SourceWebView", "Expected webView, but found none.");
                return;
            }

            JSONArray output = new JSONArray();
            selectors.forEach(selector -> {
                try {
                    output.put(selector.toJson());
                } catch (JSONException e) {
                    Log.d("SourceWebView", "Unable to convert selector to JSON.", e);
                }
            });

            String script = sourceWebViewViewModel.getHighlightSelectorScript(
                    getActivity().getAssets(), output
            );
            this.webView.evaluateJavascript(script, (result) -> {
                /** noop **/
            });
        });

        sourceWebViewViewModel.getDomainMetadata().observe(getActivity(), (domainMetadata) -> {
            if (domainMetadata != null) {
                Log.i("SourceWebView", "setTitle: " + domainMetadata.getUrl().getHost());
                toolbar.setTitle(domainMetadata.getUrl().getHost());
                Bitmap favicon = domainMetadata.getFavicon();
                int faviconSize = getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_size);
                if (favicon != null) {
                    toolbar.setLogo(new BitmapDrawable(getResources(), Bitmap.createScaledBitmap(favicon, faviconSize, faviconSize, true)));
                }
            }
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