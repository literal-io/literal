package io.literal.ui.fragment;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.google.android.material.appbar.AppBarLayout;
import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.json.JSONArray;
import org.json.JSONException;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Optional;
import java.util.UUID;

import io.literal.R;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.WebEvent;
import io.literal.model.Annotation;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;
import io.literal.viewmodel.SourceWebViewViewModel;

public class SourceWebView extends Fragment {

    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";

    private String paramInitialUrl;
    private io.literal.ui.view.SourceWebView webView;
    private Toolbar toolbar;
    private AppBarLayout appBarLayout;

    private SourceWebViewViewModel sourceWebViewViewModel;
    private AuthenticationViewModel authenticationViewModel;
    private AppWebViewViewModel appWebViewViewModel;

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
        View v = inflater.inflate(R.layout.fragment_source_web_view, container, false);
        setHasOptionsMenu(true);
        return v;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        sourceWebViewViewModel = new ViewModelProvider(requireActivity()).get(SourceWebViewViewModel.class);
        authenticationViewModel = new ViewModelProvider(requireActivity()).get(AuthenticationViewModel.class);
        appWebViewViewModel = new ViewModelProvider(requireActivity()).get(AppWebViewViewModel.class);

        toolbar = view.findViewById(R.id.toolbar);
        ((AppCompatActivity) getActivity()).setSupportActionBar(toolbar);

        appBarLayout = view.findViewById(R.id.app_bar_layout);

        webView = view.findViewById(R.id.source_web_view);
        webView.setOnReceivedIcon((e, webView, icon) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnReceivedIcon callback error:", e);
            }

            SourceWebViewViewModel.DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
            sourceWebViewViewModel.setDomainMetadata(domainMetadata != null ? domainMetadata.getUrl() : null, icon);
        });
        webView.setOnGetWebMessageChannelInitializerScript((_e, _data) -> {
            try {
                ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
                JSONArray output = JsonArrayUtil.stringifyObjectArray(annotations.toArray(new Annotation[0]), Annotation::toJson);
                String script = sourceWebViewViewModel.getHighlightAnnotationTargetScript(
                        getActivity().getAssets(), output
                );
                Log.i("SourceWebView", script);
                return script;
            } catch (JSONException e) {
                Log.d("SourceWebView", "setOnGetWebMessageChannelInitializerScript callback", e);
            }
            return null;
        });

        webView.setExternalWebViewClient(new WebViewClient() {
            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                SourceWebViewViewModel.DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
                try {
                    sourceWebViewViewModel.setDomainMetadata(new URL(url), domainMetadata != null ? domainMetadata.getFavicon() : null);
                } catch (MalformedURLException ex) {
                    Log.d("SourceWebView", "Unable to execute setDomainMetadata:", ex);
                }
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                if (!sourceWebViewViewModel.getHasFinishedInitializing().getValue()) {
                    sourceWebViewViewModel.setHasFinishedInitializing(true);
                }
            }
        });

        webView.setOnAnnotationCreated((e, _view) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnAnnotationCreated callback error:", e);
                return;
            }

            String script = sourceWebViewViewModel.getGetAnnotationScript(getActivity().getAssets());
            webView.evaluateJavascript(script, value -> {
                String creatorUsername = authenticationViewModel.getUsername().getValue();
                Annotation annotation = sourceWebViewViewModel.createAnnotation(value, creatorUsername);
                try {
                    appWebViewViewModel.dispatchWebEvent(new WebEvent(
                            WebEvent.TYPE_NEW_ANNOTATION,
                            UUID.randomUUID().toString(),
                            annotation.toJson()
                    ));
                    appWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_EXPANDED);
                } catch (JSONException e1) {
                    Log.d("SourceWebView", "Unable to stringify annotation", e1);
                }
            });
        });

        sourceWebViewViewModel.getAnnotations().observe(getActivity(), (annotations) -> {
            if (this.webView == null) {
                Log.d("SourceWebView", "Expected webView, but found none.");
                return;
            }

            try {
                JSONArray output = JsonArrayUtil.stringifyObjectArray(annotations.toArray(new Annotation[0]), Annotation::toJson);
                String script = sourceWebViewViewModel.getHighlightAnnotationTargetScript(
                        getActivity().getAssets(), output
                );
                this.webView.evaluateJavascript(script, (result) -> {
                    /** noop **/
                });
            } catch (JSONException ex) {
                Log.d("SourceWebView", "observe annotations", ex);
            }
        });

        sourceWebViewViewModel.getDomainMetadata().observe(getActivity(), (domainMetadata) -> {
            if (domainMetadata != null) {
                toolbar.setTitle(domainMetadata.getUrl().getHost());
                Bitmap favicon = domainMetadata.getFavicon();
                if (favicon != null) {
                    // scale favicon and draw on white background
                    int faviconSize = getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_size);
                    int padding = getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_padding);
                    Bitmap scaledBitmap = Bitmap.createScaledBitmap(favicon, faviconSize, faviconSize, true);
                    Bitmap outputBitmap = Bitmap.createBitmap(scaledBitmap.getWidth() + padding, scaledBitmap.getHeight() + padding, scaledBitmap.getConfig());
                    Canvas canvas = new Canvas(outputBitmap);
                    canvas.drawColor(Color.WHITE);
                    canvas.drawBitmap(scaledBitmap, (float) padding / 2, (float) padding / 2, null);

                    toolbar.setLogo(new BitmapDrawable(getResources(), outputBitmap));
                }
            }
        });
        webView.setWebEventCallback((e, webView, event) -> {
            switch (event.getType()) {
                case WebEvent.TYPE_FOCUS_ANNOTATION:
                    String annotationId = event.getData().optString("annotationId");
                    if (!annotationId.isEmpty()) {
                        handleAnnotationFocus(annotationId);
                    }
                    break;
            }
        });

        appWebViewViewModel.getReceivedWebEvents().observe(requireActivity(), (webEvents) -> {
            if (webEvents == null) {
                return;
            }

            webEvents.iterator().forEachRemaining((webEvent) -> {
                switch (webEvent.getType()) {
                    case WebEvent.TYPE_ACTIVITY_FINISH:
                        appWebViewViewModel.setBottomSheetState(
                                sourceWebViewViewModel.getFocusedAnnotation().getValue() != null ? BottomSheetBehavior.STATE_COLLAPSED : BottomSheetBehavior.STATE_HIDDEN
                        );
                        return;
                    case WebEvent.TYPE_NEW_ANNOTATION_RESULT:
                        try {
                            Annotation newAnnotation = Annotation.fromJson(webEvent.getData());
                            boolean updated = sourceWebViewViewModel.updateAnnotation(newAnnotation);
                            if (!updated) {
                                Log.d("SourceWebView", "Unable to update annotation: " + webEvent.getData());
                            }
                        } catch (JSONException e) {
                            Log.d("SourceWebView", "Unable to handle NEW_ANNOTATION_RESULT", e);
                        }
                        return;
                }
            });

            appWebViewViewModel.clearReceivedWebEvents();
        });

        appWebViewViewModel.getBottomSheetState().observe(requireActivity(), (bottomSheetState) -> {
            switch (bottomSheetState) {
                case BottomSheetBehavior.STATE_COLLAPSED:
                    appBarLayout.setExpanded(true);
                    break;
                case BottomSheetBehavior.STATE_EXPANDED:
                    appBarLayout.setExpanded(false);
                    break;
            }
        });

        if (savedInstanceState != null) {
            webView.restoreState(savedInstanceState);
        } else {
            webView.loadUrl(paramInitialUrl);
        }
    }

    private void handleAnnotationFocus(String annotationId) {
        Optional<Annotation> annotation = sourceWebViewViewModel
                .getAnnotations()
                .getValue()
                .stream()
                .filter((a) -> a.getId() != null && a.getId().equals(annotationId))
                .findFirst();

        if (!annotation.isPresent()) {
            Log.d("SourceWebView", "handleAnnotationClicked unable to find annotationId");
        }
        Annotation unwrappedAnnotation = annotation.get();

        sourceWebViewViewModel.setFocusedAnnotation(unwrappedAnnotation);
        try {
            appWebViewViewModel.dispatchWebEvent(new WebEvent(
                    WebEvent.TYPE_FOCUS_ANNOTATION,
                    UUID.randomUUID().toString(),
                    unwrappedAnnotation.toJson()
            ));
            appWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_COLLAPSED);
        } catch (JSONException e) {
            Log.d("SourceWebView", "handleAnnotationClicked unable to serialize annotation", e);
        }
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        if (this.webView != null) {
            webView.saveState(outState);
        }
    }

    @Override
    public void onCreateOptionsMenu(@NonNull Menu menu, @NonNull MenuInflater inflater) {
        inflater.inflate(R.menu.source_webview_toolbar, menu);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.done:
                // TODO: handle item selected
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}