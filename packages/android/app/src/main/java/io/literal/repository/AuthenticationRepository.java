package io.literal.repository;

import android.app.Activity;
import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.HostedUIOptions;
import com.amazonaws.mobile.client.SignInUIOptions;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.results.Tokens;

import java.util.Map;

public class AuthenticationRepository {

    public static Tokens getTokens() throws Exception {
        return AWSMobileClient.getInstance().getTokens();
    }

    public static void getTokens(Callback<Tokens> callback) {
        AWSMobileClient.getInstance().getTokens(new com.amazonaws.mobile.client.Callback<Tokens>() {
            @Override
            public void onResult(Tokens result) {
                callback.invoke(null, result);
            }

            @Override
            public void onError(Exception e) {
                callback.invoke(e, null);
            }
        });
    }

    public static Map<String, String> getUserAttributes() throws Exception {
        return AWSMobileClient.getInstance().getUserAttributes();
    }

    public static String getUsername() {
        return AWSMobileClient.getInstance().getUsername();
    }

    public static String getIdentityId() {
        return AWSMobileClient.getInstance().getIdentityId();
    }

    public static void getUserAttributes(Callback<Map<String, String>> callback) {
        AWSMobileClient.getInstance().getUserAttributes(new com.amazonaws.mobile.client.Callback<Map<String, String>>() {
            @Override
            public void onResult(Map<String, String> result) {
                callback.invoke(null, result);
            }

            @Override
            public void onError(Exception e) {
                callback.invoke(e, null);
            }
        });
    }

    public static void signInGoogle(Activity activity, Callback<UserStateDetails> callback) {
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
                callback.invoke(null, result);
            }

            @Override
            public void onError(Exception e) {
                callback.invoke(e, null);
            }
        });
    }

    public interface Callback<T> {
        void invoke(Exception e, T data);
    }
}
