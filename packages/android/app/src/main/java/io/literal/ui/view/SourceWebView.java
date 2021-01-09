package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewParent;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import org.apache.commons.io.IOUtils;

import io.literal.R;
import io.literal.lib.Callback;
import io.literal.lib.Callback2;

public class SourceWebView extends WebView {

    private Callback2<View, String> onPageFinished;
    private Callback<View> onAnnotationCreated;

    public SourceWebView(Context context) {
        super(context);
        this.initialize();
    }

    public SourceWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.initialize();
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void initialize() {
        WebSettings webSettings = this.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        this.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                if (SourceWebView.this.onPageFinished != null) {
                    onPageFinished.invoke(null, view, url);
                }
            }
        });
    }

    @Override
    public ActionMode startActionMode(ActionMode.Callback callback) {
        ActionMode.Callback2 cb = new AnnotateActionModeCallback((ActionMode.Callback2) callback);
        return super.startActionMode(cb);
    }

    @Override
    public ActionMode startActionMode(ActionMode.Callback callback, int type) {
        ActionMode.Callback2 cb = new AnnotateActionModeCallback((ActionMode.Callback2) callback);
        return super.startActionMode(cb, type);
    }

    public void setOnPageFinished(Callback2<View, String> onPageFinished) {
        this.onPageFinished = onPageFinished;
    }

    public void setOnAnnotationCreated(Callback<View> onAnnotationCreated) {
        this.onAnnotationCreated = onAnnotationCreated;
    }

    private class AnnotateActionModeCallback extends ActionMode.Callback2 {

        ActionMode.Callback2 originalCallback;

        public AnnotateActionModeCallback(ActionMode.Callback2 originalCallback) {
            this.originalCallback = originalCallback;
        }

        @Override
        public boolean onCreateActionMode(ActionMode mode, Menu menu) {
            MenuInflater inflater = mode.getMenuInflater();
            inflater.inflate(R.menu.source_webview_menu, menu);
            return true;
        }

        @Override
        public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
            return false;
        }

        @Override
        public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
            switch (item.getItemId()) {
                case R.id.menu_item_annotate:
                    if (onAnnotationCreated != null) {
                        onAnnotationCreated.invoke(null, SourceWebView.this);
                    }
                    mode.finish();
                    return true;
                default:
                    return false;
            }
        }

        @Override
        public void onDestroyActionMode(ActionMode mode) {
        }

        @Override
        public void onGetContentRect(ActionMode mode, View view, Rect outRect) {
            originalCallback.onGetContentRect(mode, view, outRect);
        }
    };
}
