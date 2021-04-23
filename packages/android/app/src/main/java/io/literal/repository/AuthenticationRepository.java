package io.literal.repository;

import android.app.Activity;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.os.NetworkOnMainThreadException;
import android.util.Log;

import com.amazonaws.auth.AwsChunkedEncodingInputStream;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.HostedUIOptions;
import com.amazonaws.mobile.client.SignInUIOptions;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.results.SignInResult;
import com.amazonaws.mobile.client.results.SignUpResult;
import com.amazonaws.mobile.client.results.Tokens;

import java.security.NoSuchAlgorithmException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import io.literal.lib.Crypto;

public class AuthenticationRepository {

    public static void getTokens(Callback<Tokens> callback) {
        try {
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
        } catch (Exception e) {
            callback.invoke(e, null);
        }
    }

    public static String getUsername() {
        try {
            Map<String, String> userAttributes = AWSMobileClient.getInstance().getUserAttributes();
            String username = AWSMobileClient.getInstance().getUsername();
            if (username == null) {
                return null;
            }
            return username.startsWith("Google") ? username : userAttributes.get("sub");
        } catch (Exception e) {
            ErrorRepository.captureException(e);
            return null;
        }
    }

    public static void getUsername(Callback<String> callback) {
        AWSMobileClient.getInstance().getUserAttributes(new com.amazonaws.mobile.client.Callback<Map<String, String>>() {
            @Override
            public void onResult(Map<String, String> result) {
                String username = AWSMobileClient.getInstance().getUsername();
                if (username == null) {
                    callback.invoke(null, null);
                    return;
                }
                callback.invoke(null, username.startsWith("Google") ? username : result.get("sub"));
            }

            @Override
            public void onError(Exception e) {
                callback.invoke(e, null);
            }
        });
    ;}

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
        /*
         * Resolve the browser to use. 1. Default. 2. Any app that can match intent. 3. com.android.chrome
         * Note that this will cause an exception if com.android.chrome is used without being installed.
         */
        Intent browserIntent = new Intent("android.intent.action.VIEW", Uri.parse("https://literal.io"));
        String browserPackage = activity.getPackageManager().queryIntentActivities(browserIntent, PackageManager.MATCH_DEFAULT_ONLY)
                .stream()
                .findFirst()
                .map((info) -> info.activityInfo.packageName)
                .orElseGet(() -> {
                    browserIntent.setAction(Intent.CATEGORY_BROWSABLE);
                    return activity.getPackageManager().queryIntentActivities(browserIntent, PackageManager.MATCH_ALL)
                            .stream()
                            .findFirst()
                            .map((info -> info.activityInfo.packageName))
                            .orElse("com.android.chrome");
                });

        HostedUIOptions hostedUIOptions = HostedUIOptions.builder()
                .scopes("openid", "email", "phone", "profile", "aws.cognito.signin.user.admin")
                .identityProvider("Google")
                .build();
        SignInUIOptions signInUIOptions = SignInUIOptions.builder()
                .hostedUIOptions(hostedUIOptions)
                .canCancel(false)
                .browserPackage(browserPackage)
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

    public static void signIn(String email, String password, io.literal.lib.Callback<Exception, UserStateDetails> callback) {
        AWSMobileClient.getInstance().signIn(email, password, null, new com.amazonaws.mobile.client.Callback<SignInResult>() {
            @Override
            public void onResult(SignInResult result) {
                AWSMobileClient.getInstance().currentUserState(new com.amazonaws.mobile.client.Callback<UserStateDetails>() {
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
            @Override
            public void onError(Exception e) {
                callback.invoke(e, null);
            }
        });
    }

    public static void signUp(String email, String password, Callback<UserStateDetails> callback) {
        final Map<String, String> attributes = new HashMap<>();
        attributes.put("email", email);
        AWSMobileClient.getInstance().signUp(email, password, attributes, null, new com.amazonaws.mobile.client.Callback<SignUpResult>() {
            @Override
            public void onResult(SignUpResult result) {
                // User is auto confirmed
                AWSMobileClient.getInstance().signIn(email, password, null, new com.amazonaws.mobile.client.Callback<SignInResult>() {
                    @Override
                    public void onResult(SignInResult result) {
                        AWSMobileClient.getInstance().currentUserState(new com.amazonaws.mobile.client.Callback<UserStateDetails>() {
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
                    @Override
                    public void onError(Exception e) {
                        callback.invoke(e, null);
                    }
                });
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
