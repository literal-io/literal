package io.literal.ui.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;

import io.literal.lib.Constants;

public class AuthenticationHandler extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Intent activityIntent = getIntent();
        if (activityIntent.getData() != null &&
                "literal".equals(activityIntent.getData().getScheme())) {
            Log.d(Constants.LOG_TAG, "authHandler: " + ((Uri) activityIntent.getData()).toString());
            AWSMobileClient.getInstance().handleAuthResponse(activityIntent);
        }
    }
}
