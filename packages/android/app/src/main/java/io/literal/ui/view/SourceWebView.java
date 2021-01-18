package io.literal.ui.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.core.view.MotionEventCompat;
import androidx.core.view.NestedScrollingChild;
import androidx.core.view.NestedScrollingChildHelper;
import androidx.core.view.ViewCompat;

import org.w3c.dom.Attr;

import java.text.AttributedCharacterIterator;

import io.literal.R;
import io.literal.lib.Callback;
import io.literal.lib.Callback2;
import io.literal.lib.Callback3;

public class SourceWebView extends WebView implements NestedScrollingChild {

    private Callback3<View, String, Bitmap> onPageStarted;
    private Callback2<View, String> onPageFinished;
    private Callback<View> onAnnotationCreated;
    private Callback2<View, Bitmap> onReceivedIcon;

    // NestedScrollingChild
    private int mLastY;
    private final int[] mScrollOffset = new int[2];
    private final int[] mScrollConsumed = new int[2];
    private int mNestedOffsetY;
    private NestedScrollingChildHelper mChildHelper;

    public SourceWebView(Context context) {
        this(context, null);

    }

    public SourceWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mChildHelper = new NestedScrollingChildHelper(this);
        setNestedScrollingEnabled(true);
        this.initialize();
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void initialize() {
        WebSettings webSettings = this.getSettings();
        webSettings.setJavaScriptEnabled(true);
        webSettings.setDomStorageEnabled(true);
        webSettings.setBuiltInZoomControls(true);
        webSettings.setDisplayZoomControls(false);
        this.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                super.onPageStarted(view, url, favicon);
                if (SourceWebView.this.onPageStarted != null) {
                    onPageStarted.invoke(null, view, url, favicon);
                }
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                if (SourceWebView.this.onPageFinished != null) {
                    onPageFinished.invoke(null, view, url);
                }
            }
        });

        this.setWebChromeClient(new WebChromeClient() {
            @Override
            public void onReceivedIcon(WebView view, Bitmap icon) {
                if (SourceWebView.this.onReceivedIcon != null) {
                    onReceivedIcon.invoke(null, view, icon);
                }
                super.onReceivedIcon(view, icon);
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

    public void setOnPageStarted(Callback3<View, String, Bitmap> onPageStarted) {
        this.onPageStarted = onPageStarted;
    }

    public void setOnReceivedIcon(Callback2<View, Bitmap> onReceivedIcon) {
        this.onReceivedIcon = onReceivedIcon;
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

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        boolean returnValue = false;

        MotionEvent event = MotionEvent.obtain(ev);
        final int action = MotionEventCompat.getActionMasked(event);
        if (action == MotionEvent.ACTION_DOWN) {
            mNestedOffsetY = 0;
        }
        int eventY = (int) event.getY();
        event.offsetLocation(0, mNestedOffsetY);
        switch (action) {
            case MotionEvent.ACTION_MOVE:
                int deltaY = mLastY - eventY;
                // NestedPreScroll
                if (dispatchNestedPreScroll(0, deltaY, mScrollConsumed, mScrollOffset)) {
                    deltaY -= mScrollConsumed[1];
                    mLastY = eventY - mScrollOffset[1];
                    event.offsetLocation(0, -mScrollOffset[1]);
                    mNestedOffsetY += mScrollOffset[1];
                }
                returnValue = super.onTouchEvent(event);

                // NestedScroll
                if (dispatchNestedScroll(0, mScrollOffset[1], 0, deltaY, mScrollOffset)) {
                    event.offsetLocation(0, mScrollOffset[1]);
                    mNestedOffsetY += mScrollOffset[1];
                    mLastY -= mScrollOffset[1];
                }
                break;
            case MotionEvent.ACTION_DOWN:
                returnValue = super.onTouchEvent(event);
                mLastY = eventY;
                // start NestedScroll
                startNestedScroll(ViewCompat.SCROLL_AXIS_VERTICAL);
                break;
            default:
                returnValue = super.onTouchEvent(event);
                // end NestedScroll
                stopNestedScroll();
                break;
        }
        return returnValue;
    }

    // Nested Scroll implements
    @Override
    public void setNestedScrollingEnabled(boolean enabled) {
        mChildHelper.setNestedScrollingEnabled(enabled);
    }

    @Override
    public boolean isNestedScrollingEnabled() {
        return mChildHelper.isNestedScrollingEnabled();
    }

    @Override
    public boolean startNestedScroll(int axes) {
        return mChildHelper.startNestedScroll(axes);
    }

    @Override
    public void stopNestedScroll() {
        mChildHelper.stopNestedScroll();
    }

    @Override
    public boolean hasNestedScrollingParent() {
        return mChildHelper.hasNestedScrollingParent();
    }

    @Override
    public boolean dispatchNestedScroll(int dxConsumed, int dyConsumed, int dxUnconsumed, int dyUnconsumed,
                                        int[] offsetInWindow) {
        return mChildHelper.dispatchNestedScroll(dxConsumed, dyConsumed, dxUnconsumed, dyUnconsumed, offsetInWindow);
    }

    @Override
    public boolean dispatchNestedPreScroll(int dx, int dy, int[] consumed, int[] offsetInWindow) {
        return mChildHelper.dispatchNestedPreScroll(dx, dy, consumed, offsetInWindow);
    }

    @Override
    public boolean dispatchNestedFling(float velocityX, float velocityY, boolean consumed) {
        return mChildHelper.dispatchNestedFling(velocityX, velocityY, consumed);
    }

    @Override
    public boolean dispatchNestedPreFling(float velocityX, float velocityY) {
        return mChildHelper.dispatchNestedPreFling(velocityX, velocityY);
    }
}
