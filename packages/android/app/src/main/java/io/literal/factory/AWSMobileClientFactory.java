package io.literal.factory;

import android.content.Context;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;

public class AWSMobileClientFactory {

    private static volatile boolean hasInitialized = false;

    public static AWSMobileClient getInstance(Context context) {

        if (!hasInitialized) {
            AWSMobileClient.getInstance().initialize(context, new Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    hasInitialized = true;
                }

                @Override
                public void onError(Exception e) {

                }
            });
        }
        return AWSMobileClient.getInstance();
    }
}
