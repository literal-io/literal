package io.literal.lib;

import android.app.Activity;
import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.HostedUIOptions;
import com.amazonaws.mobile.client.SignInUIOptions;
import com.amazonaws.mobile.client.UserStateDetails;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.factory.AWSMobileClientFactory;

public class WebEvent {

    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";
    public static final String TYPE_ROUTER_REPLACE = "ROUTER_REPLACE";
    public static final String TYPE_SIGN_IN_GOOGLE = "SIGN_IN_GOOGLE";
    public static final String TYPE_SIGN_IN_RESULT = "SIGN_IN_RESULT";

    private String type;
    private String pid;
    private JSONObject data = new JSONObject();

    public WebEvent (JSONObject data) {
        this.type = data.optString("type");
        this.pid = data.optString("pid");
    }

    public WebEvent (String type, String pid, JSONObject data) {
        this.type = type;
        this.pid = pid;
        this.data = data;
    }

    public JSONObject toJSON() {
        try {
            JSONObject val = new JSONObject();
            val.put("type", type);
            val.put("pid", pid);
            val.put("data", data);
            return val;
        } catch (JSONException ex) { return null; }
    }

    public String getType() {
        return type;
    }


    public static class Callback {
        private final Activity activity;

        public Callback (Activity activity) {
            this.activity = activity;
        }

        private void handleSignInGoogle() {
            HostedUIOptions hostedUIOptions = HostedUIOptions.builder()
                    .scopes("openid", "email")
                    .identityProvider("Google")
                    .build();
            SignInUIOptions signInUIOptions = SignInUIOptions.builder()
                    .hostedUIOptions(hostedUIOptions)
                    .build();
            AWSMobileClientFactory.getInstance(activity).showSignIn(activity, signInUIOptions, new com.amazonaws.mobile.client.Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    Log.d(Constants.LOG_TAG, "onResult: "  + result.getUserState());
                    Log.d(Constants.LOG_TAG, "onResult details: " + result.getDetails().toString());
                }
                @Override
                public void onError(Exception e) {
                    Log.e(Constants.LOG_TAG, "onError: ", e);
                }
            });
        }

        public void onWebEvent(WebEvent event) {
            switch (event.getType()) {
                case WebEvent.TYPE_SIGN_IN_GOOGLE:
                    this.handleSignInGoogle();
                    return;
            }
        }
    }
}