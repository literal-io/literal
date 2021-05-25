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
import com.amazonaws.mobile.client.SignOutOptions;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.results.SignInResult;
import com.amazonaws.mobile.client.results.SignUpResult;
import com.amazonaws.mobile.client.results.Token;
import com.amazonaws.mobile.client.results.Tokens;

import net.jodah.failsafe.AsyncExecution;
import net.jodah.failsafe.Failsafe;
import net.jodah.failsafe.RetryPolicy;
import net.jodah.failsafe.Timeout;
import net.jodah.failsafe.function.AsyncSupplier;

import java.net.UnknownHostException;
import java.security.NoSuchAlgorithmException;
import java.time.Duration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CompletionStage;
import java.util.function.BiConsumer;

import io.literal.lib.Crypto;
import io.sentry.SentryLevel;

public class AuthenticationRepository {

    public static class InvalidStateException extends Exception {
        public InvalidStateException(String message) {
            super(message);
        }
    }

    public static final RetryPolicy<Object> retryPolicy = new RetryPolicy<>()
            .withMaxAttempts(3)
            .handle(UnknownHostException.class, InvalidStateException.class);

    public static CompletableFuture<Tokens> getTokens() {
        CompletableFuture<Tokens> future = new CompletableFuture<>();
        AWSMobileClient.getInstance().getTokens(new com.amazonaws.mobile.client.Callback<Tokens>() {
            @Override
            public void onResult(Tokens result) {
                future.complete(result);
            }

            @Override
            public void onError(Exception e) {
                future.completeExceptionally(e);
            }
        });

        return future;
    }

    public static CompletableFuture<Tokens> getTokensWithRetry() {
        return Failsafe.with(retryPolicy).getStageAsyncExecution(execution -> getTokens().whenComplete((tokens, throwable) -> {
            if (execution.complete(tokens, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Tokens retrieved successfully.", SentryLevel.INFO);
            } else if (!execution.retryFor(tokens, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Failed to retrieve tokens.", SentryLevel.ERROR);
            }
        }));
    }

    public static CompletableFuture<Map<String, String>> getUserAttributes() {
        CompletableFuture<Map<String, String>> future = new CompletableFuture<>();
        AWSMobileClient.getInstance().getUserAttributes(new com.amazonaws.mobile.client.Callback<Map<String, String>>() {
            @Override
            public void onResult(Map<String, String> result) {
                future.complete(result);
            }

            @Override
            public void onError(Exception e) {
                future.completeExceptionally(e);
            }
        });
        return future;
    }

    public static CompletableFuture<Map<String, String>> getUserAttributesWithRetry() {
        return Failsafe.with(retryPolicy).getStageAsyncExecution(execution -> getUserAttributes().whenComplete((userAttributes, throwable) -> {
            if (execution.complete(userAttributes, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "User attributes retrieved successfully.", SentryLevel.INFO);
            } else if (!execution.retryFor(userAttributes, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Failed to retrieve user attributes.", SentryLevel.ERROR);
            }
        }));
    }

    public static CompletableFuture<String> getUsername() {
        return CompletableFuture.completedFuture(AWSMobileClient.getInstance().getUsername());
    }

    public static CompletableFuture<String> getUsernameWithRetry() {
        return Failsafe.with(retryPolicy).getStageAsyncExecution(execution -> getUsername().whenComplete((username, throwable) -> {
            if (execution.complete(username, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Username retrieved successfully: " + username, SentryLevel.INFO);
            } else if (!execution.retryFor(username, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Failed to retrieve username.", SentryLevel.ERROR);
            }
        }));
    }

    public static CompletableFuture<String> getIdentityId() {
        CompletableFuture<String> future = new CompletableFuture<>();
        String identityId = AWSMobileClient.getInstance().getIdentityId();
        if (identityId == null) {
            future.completeExceptionally(new InvalidStateException("Identity ID is null"));
            return future;
        }
        future.complete(identityId);
        return future;
    }

    public static CompletableFuture<String> getIdentityIdWithRetry() {
        return Failsafe.with(retryPolicy).getStageAsyncExecution(execution -> getIdentityId().whenComplete((identityId, throwable) -> {
            if (execution.complete(identityId, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Identity ID retrieved successfully: " + identityId, SentryLevel.INFO);
            } else if (!execution.retryFor(identityId, throwable)) {
                ErrorRepository.captureBreadcrumb(ErrorRepository.CATEGORY_AUTHENTICATION, "Failed to retrieve Identity ID.", SentryLevel.ERROR);

            }
        }));
    }

    public static CompletableFuture<Void> signOut() {
        CompletableFuture<Void> future = new CompletableFuture<>();
        AWSMobileClient.getInstance().signOut(SignOutOptions.builder().build(), new com.amazonaws.mobile.client.Callback<Void>() {
            @Override
            public void onResult(Void result) {
                future.complete(null);
            }

            @Override
            public void onError(Exception e) {
                future.completeExceptionally(e);
            }
        });
        return future;
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
