package io.literal.ui.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.Nullable;

import org.json.JSONArray;
import org.json.JSONException;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import io.literal.lib.DomainMetadata;
import io.literal.lib.JsonArrayUtil;
import io.literal.model.Annotation;
import io.literal.model.StorageObject;
import io.literal.repository.ErrorRepository;
import io.literal.repository.StorageRepository;
import io.literal.viewmodel.AuthenticationViewModel;
import io.literal.viewmodel.SourceWebViewViewModel;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.functions.Function3;

public class SourceWebViewClient extends WebViewClient {
    private final Context context;
    private final AuthenticationViewModel authenticationViewModel;
    private final SourceWebViewViewModel sourceWebViewViewModel;
    private boolean shouldClearHistoryOnPageFinished;

    public SourceWebViewClient(
            Context context,
            AuthenticationViewModel authenticationViewModel,
            SourceWebViewViewModel sourceWebViewViewModel
        ) {
        this.context = context;
        this.authenticationViewModel = authenticationViewModel;
        this.sourceWebViewViewModel = sourceWebViewViewModel;
        this.shouldClearHistoryOnPageFinished = false;
    }

    @Nullable
    @Override
    public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
        if (request == null || !request.getMethod().equals("GET")) {
            return null;
        }

        return StorageObject.create(context, request.getUrl())
                .flatMap((storageObject) -> {
                    storageObject.ensureDownloadRequired(context);
                    try {
                        WebResourceResponse response = new WebResourceResponse(
                                storageObject.getContentType(context).toMimeType(),
                                null,
                                storageObject.downloadAsInputStream(context, authenticationViewModel.getUser().getValue())
                        );
                        Map<String, String> responseHeaders = new HashMap<>();
                        responseHeaders.put("Cache-Control", StorageObject.CACHE_CONTROL);
                        response.setResponseHeaders(responseHeaders);
                        return Optional.of(response);
                    } catch (Exception e) {
                        ErrorRepository.captureException(e);
                        return Optional.empty();
                    }
                })
                .orElse(null);
    }

    @Override
    public void onPageFinished(WebView webview, String url) {
        if (webview.getProgress() == 100) {

            // Ensure web context is setup to begin the message channel handshake.
            if (!sourceWebViewViewModel.getHasInjectedAnnotationRendererScript().getValue()) {
                sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(true);
                webview.evaluateJavascript(this.getAnnotationRendererScript(), (value -> { /** noop **/}));
            }

            if (shouldClearHistoryOnPageFinished) {
                webview.clearHistory();
                shouldClearHistoryOnPageFinished = false;
            }
        }
    }

    @Override
    public void onPageStarted(WebView webview, String url, Bitmap favicon) {
        try {
            URL newUrl = new URL(url);
            DomainMetadata domainMetadata = Optional.ofNullable(sourceWebViewViewModel.getDomainMetadata().getValue())
                    .map(inst -> DomainMetadata.updateUrl(inst, newUrl))
                    .orElse(new DomainMetadata(newUrl, null));
            sourceWebViewViewModel.setDomainMetadata(domainMetadata);
        } catch (MalformedURLException ex) {
            ErrorRepository.captureException(ex);
        }
        sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(false);
    }

    public void setShouldClearHistoryOnPageFinished(boolean shouldClearHistoryOnPageFinished) {
        this.shouldClearHistoryOnPageFinished = shouldClearHistoryOnPageFinished;
    }

    private String getAnnotationRendererScript() {
        ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
        String focusedAnnotationId = sourceWebViewViewModel.getFocusedAnnotationId().getValue();

        try {
            JSONArray paramAnnotations = JsonArrayUtil.stringifyObjectArray(annotations.toArray(new Annotation[0]), Annotation::toJson);
            return sourceWebViewViewModel.getAnnotationRendererScript(
                    context.getAssets(),
                    paramAnnotations,
                    focusedAnnotationId != null ? focusedAnnotationId : ""
            );
        } catch (JSONException ex) {
            ErrorRepository.captureException(ex);
            return null;
        }
    }
}
