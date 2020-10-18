package io.literal.lib;

import android.app.Activity;
import android.os.AsyncTask;
import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.HostedUIOptions;
import com.amazonaws.mobile.client.SignInUIOptions;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.results.Tokens;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;

import io.literal.ui.view.WebView;

public class WebEvent {

    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";
    public static final String TYPE_ROUTER_REPLACE = "ROUTER_REPLACE";

    public static final String TYPE_AUTH_SIGN_IN = "AUTH_SIGN_IN";
    public static final String TYPE_AUTH_SIGN_IN_RESULT = "AUTH_SIGN_IN_RESULT";

    public static final String TYPE_AUTH_GET_TOKENS = "AUTH_GET_TOKENS";
    public static final String TYPE_AUTH_GET_TOKENS_RESULT = "AUTH_GET_TOKENS_RESULT";

    public static final String TYPE_AUTH_GET_USER_INFO = "AUTH_GET_USER_INFO";
    public static final String TYPE_AUTH_GET_USER_INFO_RESULT = "AUTH_GET_USER_INFO_RESULT";

    private String type;
    private String pid;
    private JSONObject data = new JSONObject();

    public WebEvent(JSONObject data) {
        this.type = data.optString("type");
        this.pid = data.optString("pid");
    }

    public WebEvent(String type, String pid, JSONObject data) {
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
        } catch (JSONException ex) {
            return null;
        }
    }

    public String getType() {
        return type;
    }


    public static class Callback {
        private final Activity activity;
        private final WebView webView;

        public Callback(Activity activity, WebView webView) {
            this.activity = activity;
            this.webView = webView;
        }

        private void handleSignInGoogle() {
            HostedUIOptions hostedUIOptions = HostedUIOptions.builder()
                    .scopes("openid", "email", "phone", "profile", "aws.cognito.signin.user.admin")
                    .identityProvider("Google")
                    .build();
            SignInUIOptions signInUIOptions = SignInUIOptions.builder()
                    .hostedUIOptions(hostedUIOptions)
                    .canCancel(false)
                    .build();
            AWSMobileClient.getInstance().showSignIn(activity, signInUIOptions, new com.amazonaws.mobile.client.Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    activity.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                Tokens tokens = AWSMobileClient.getInstance().getTokens();
                                JSONObject result = new JSONObject();
                                result.put("idToken", tokens.getIdToken().getTokenString());
                                result.put("refreshToken", tokens.getRefreshToken().getTokenString());
                                result.put("accessToken", tokens.getAccessToken().getTokenString());
                                webView.postWebEvent(
                                        new WebEvent(WebEvent.TYPE_AUTH_SIGN_IN_RESULT, UUID.randomUUID().toString(), result)
                                );
                            } catch (Exception e) {
                                Log.e(Constants.LOG_TAG, "Unable to handleSignGoogle: ", e);
                            }
                            ;

                        }
                    });

                }

                @Override
                public void onError(Exception e) {
                    Log.e(Constants.LOG_TAG, "onError: ", e);
                }
            });
        }

        public void onWebEvent(WebEvent event) {
            switch (event.getType()) {
                case WebEvent.TYPE_AUTH_SIGN_IN:
                    this.handleSignInGoogle();
                    return;
                case WebEvent.TYPE_AUTH_GET_TOKENS:
                    new GetTokensHandlerTask().execute();
                    return;
                case WebEvent.TYPE_AUTH_GET_USER_INFO:
                    new GetUserInfoHandlerTask().execute();
                    return;
            }
        }

        private class GetTokensHandlerTask extends AsyncTask<Void, Void, JSONObject> {
            @Override
            protected JSONObject doInBackground(Void... voids) {
                try {
                    JSONObject result = new JSONObject();
                    Tokens tokens = AWSMobileClient.getInstance().getTokens();
                    result.put("idToken", tokens.getIdToken().getTokenString());
                    result.put("refreshToken", tokens.getRefreshToken().getTokenString());
                    result.put("accessToken", tokens.getAccessToken().getTokenString());
                    return result;
                } catch (Exception e) {
                    Log.e(Constants.LOG_TAG, "Unable to handleGetTokens: ", e);
                    Log.i(Constants.LOG_TAG, "Signing user out.");
                    AWSMobileClient.getInstance().signOut();
                    return null;
                }
            }

            @Override
            protected void onPostExecute(JSONObject result) {
                webView.postWebEvent(
                        new WebEvent(WebEvent.TYPE_AUTH_GET_TOKENS_RESULT, UUID.randomUUID().toString(), result)
                );
            }
        }

        private class GetUserInfoHandlerTask extends AsyncTask<Void, Void, JSONObject> {
            @Override
            protected JSONObject doInBackground(Void... voids) {
                try {
                    AWSMobileClient awsMobileClient = AWSMobileClient.getInstance();
                    JSONObject result = new JSONObject();
                    result.put("username", awsMobileClient.getUsername());

                    // FIXME: getUserAttributes is slow - separate network call?
                    result.put("attributes", new JSONObject(/*awsMobileClient.getUserAttributes()*/));
                    result.put("id", awsMobileClient.getIdentityId());

                    Log.i(Constants.LOG_TAG, "GetUserInfo: " + result.toString());
                    return result;

                } catch (Exception e) {
                    Log.e(Constants.LOG_TAG, "Unable to handleGetUserInfo: ", e);
                    return null;
                }
            }

            @Override
            protected void onPostExecute(JSONObject result) {
                webView.postWebEvent(
                        new WebEvent(WebEvent.TYPE_AUTH_GET_USER_INFO_RESULT, UUID.randomUUID().toString(), result)
                );
            }
        }
    }
}