package io.literal.lib;

import android.content.Context;
import android.net.Uri;
import android.util.Log;

import org.apache.commons.io.FileUtils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

public class ContentResolverLib {
    public static File toFile (Context context, Uri uri, String fileName) {
        try {
            InputStream inputStream = context.getContentResolver().openInputStream(uri);
            File outputFile = new File(context.getFilesDir(), fileName);
            FileUtils.copyInputStreamToFile(inputStream, outputFile);
            return outputFile;
        } catch (IOException e) {
            Log.e("", "Failed to perform ContentResolverLib.toFile", e);
            return null;
        }
    }
}
