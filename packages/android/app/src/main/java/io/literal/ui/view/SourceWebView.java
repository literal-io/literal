package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Rect;
import android.util.Log;
import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewParent;
import android.webkit.WebSettings;

import io.literal.R;

public class SourceWebView extends WebView {

    public SourceWebView(Context context) {
        super(context);
        this.initialize();
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void initialize() {
        WebSettings webSettings = this.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
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

    private static class AnnotateActionModeCallback extends ActionMode.Callback2 {

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
                    Log.d("SourceWebView", "menu_item_annotate clicked");
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
