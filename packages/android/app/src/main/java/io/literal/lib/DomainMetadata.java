package io.literal.lib;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.util.Log;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URL;
import java.util.UUID;

import io.literal.R;
import io.literal.repository.ErrorRepository;

public class DomainMetadata {

    // In the case of a cached source, we want to show the original content host.
    private final URL cannonicalUrl;
    private final URL url;
    private final Bitmap favicon;
    private final URI faviconUri;

    public DomainMetadata(URL url, URL displayUrl, Bitmap favicon) {
        this.url = url;
        this.cannonicalUrl = displayUrl;
        this.favicon = favicon;
        this.faviconUri = null;
    }

    public DomainMetadata(URL url, Bitmap favicon) {
       this(url, url, favicon);
    }

    public Bitmap getFavicon() {
        return favicon;
    }
    public URL getUrl() {
        return url;
    }
    public URL getCannonicalUrl() { return cannonicalUrl; }

    public static DomainMetadata updateFavicon(@NotNull DomainMetadata inst, Bitmap newFavicon) {
        return new DomainMetadata(
                inst.url,
                inst.cannonicalUrl,
                newFavicon
        );
    }

    public static DomainMetadata updateUrl(@NotNull DomainMetadata inst, URL newUrl, URL newCannonicalUrl) {
        if (!newUrl.toString().equals(inst.url.toString())) {
            return new DomainMetadata(
                    newUrl,
                    newCannonicalUrl,
                    inst.favicon
            );
        }
        return inst;
    }

    public static DomainMetadata updateUrl(DomainMetadata inst, URL newUrl) {
        return DomainMetadata.updateUrl(inst, newUrl, newUrl);
    }

    public Bitmap getScaledFaviconWithBackground(Context context) {
        if (this.favicon == null) {
            return null;
        }
        int faviconSize = context.getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_size);
        int padding = context.getResources().getDimensionPixelSize(R.dimen.source_web_view_favicon_padding);
        Bitmap scaledBitmap = Bitmap.createScaledBitmap(favicon, faviconSize, faviconSize, true);
        Bitmap outputBitmap = Bitmap.createBitmap(scaledBitmap.getWidth() + padding, scaledBitmap.getHeight() + padding, scaledBitmap.getConfig());
        Canvas canvas = new Canvas(outputBitmap);
        canvas.drawColor(Color.WHITE);
        canvas.drawBitmap(scaledBitmap, (float) padding / 2, (float) padding / 2, null);

        return outputBitmap;
    }

    public JSONObject toJson(Context context) {
        try {
            JSONObject output = new JSONObject();
            output.put("url", url);
            output.put("cannonicalUrl", cannonicalUrl);

            if (faviconUri == null && favicon != null) {
                File outputDir = new File(context.getCacheDir(), "favicons");
                outputDir.mkdir();
                File outputFile = new File(outputDir, UUID.randomUUID().toString());
                OutputStream outputStream = new FileOutputStream(outputFile);
                favicon.compress(Bitmap.CompressFormat.WEBP, 100, outputStream);
                outputStream.flush();
                outputStream.close();
                output.put("favicon", outputFile.getAbsolutePath());
            }

            return output;
        } catch (JSONException | IOException ex) {
            ErrorRepository.captureException(ex);
            return null;
        }
    }

    public static DomainMetadata fromJson(JSONObject json) {
        try {
            Bitmap favicon = null;
            if (!json.isNull("favicon")) {
                favicon = BitmapFactory.decodeFile(json.getString("favicon"));
            }

            return new DomainMetadata(
                    !json.isNull("url") ? new URL(json.getString("url")) : null,
                    !json.isNull("displayUrl") ? new URL(json.getString("displayUrl")) : null,
                    favicon
            );
        } catch (JSONException | MalformedURLException ex) {
            ErrorRepository.captureException(ex, json.toString());
            return null;
        }
    }
}
