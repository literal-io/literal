package io.literal.ui.activity;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.UserStateListener;

import io.literal.R;
import io.literal.lib.Constants;

public class AuthenticationHandler extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.authentication_handler);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Intent activityIntent = getIntent();
        if (activityIntent.getData() != null &&
                "literal".equals(activityIntent.getData().getScheme())) {
            AWSMobileClient.getInstance().handleAuthResponse(activityIntent);

            AWSMobileClient.getInstance().addUserStateListener(new UserStateListener() {
                @Override
                public void onUserStateChanged(UserStateDetails details) {
                    switch (details.getUserState()) {
                        case SIGNED_IN:
                            Intent intent = new Intent(AuthenticationHandler.this, MainActivity.class);
                            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_TASK_ON_HOME);
                            startActivity(intent);
                            AuthenticationHandler.this.finish();
                            return;
                        default:
                            Log.d(Constants.LOG_TAG, "Expected SIGNED_IN userState, got: " + details.getUserState());
                    }
                }
            });
        }
    }
}
