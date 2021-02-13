package io.literal.lib;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;

import java.net.URL;

import io.literal.R;

public class DomainMetadata {
    private final URL url;
    private final Bitmap favicon;

    public DomainMetadata(URL url, Bitmap favicon) {
        this.url = url;
        this.favicon = favicon;
    }

    public Bitmap getFavicon() {
        return favicon;
    }

    public URL getUrl() {
        return url;
    }

    public Bitmap getScaledFaviconWithBackground(Context context) {
        int faviconSize = context.getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_size);
        int padding = context.getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_padding);
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(favicon, faviconSize, faviconSize, true);
        Bitmap outputBitmap = Bitmap.createBitmap(scaledBitmap.getWidth() + padding, scaledBitmap.getHeight() + padding, scaledBitmap.getConfig());
        Canvas canvas = new Canvas(outputBitmap);
        canvas.drawColor(Color.WHITE);
        canvas.drawBitmap(scaledBitmap, (float) padding / 2, (float) padding / 2, null);

        return outputBitmap;
    }
}
