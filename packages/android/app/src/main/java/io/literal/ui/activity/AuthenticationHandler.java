package io.literal.ui.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;

import com.amazonaws.mobile.client.AWSMobileClient;

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
            AWSMobileClient.getInstance().handleAuthResponse(activityIntent);
        }
    }
}
