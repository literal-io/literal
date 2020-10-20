package io.literal.lib;

import android.net.Uri;
import android.util.Log;
import android.webkit.ValueCallback;

import androidx.activity.result.ActivityResultCallback;

import java.io.File;

public class FileActivityResultCallback implements ActivityResultCallback<Uri> {
    ValueCallback<Uri[]> callback;

    @Override
    public void onActivityResult(Uri result) {
        if (callback != null) {
            callback.onReceiveValue(new Uri[]{result});
        } else {
            Log.i(Constants.LOG_TAG, "Received onActivityResult: " + result + ", but no callback set.");
        }
    }

    public void setFilePathCallback(ValueCallback<Uri[]> callback) {
        this.callback = callback;
    }
}
