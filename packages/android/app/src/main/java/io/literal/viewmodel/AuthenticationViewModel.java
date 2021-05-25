package io.literal.viewmodel;

import android.app.Activity;
import android.app.Application;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.amazonaws.auth.AwsChunkedEncodingInputStream;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.UserStateListener;
import com.amazonaws.mobile.client.results.Token;
import com.amazonaws.mobile.client.results.Tokens;
import com.amazonaws.mobileconnectors.cognitoauth.AuthClient;

import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ThreadPoolExecutor;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Box;
import io.literal.lib.ManyCallback;
import io.literal.model.User;
import io.literal.repository.AuthenticationRepository;
import io.literal.repository.ErrorRepository;

public class AuthenticationViewModel extends ViewModel {

    private final MutableLiveData<User> user = new MutableLiveData<>(new User(UserState.UNKNOWN));
    private UserStateListener userStateListener;
    private CompletableFuture<User> initializedFuture;

    public CompletableFuture<User> initialize(Activity activity) {
        if (initializedFuture == null) {
            initializedFuture = AWSMobileClientFactory.initializeClient(activity.getApplicationContext()).thenCompose(User::getInstance);

            initializedFuture.whenComplete((instance, error) -> activity.runOnUiThread(() -> {
                userStateListener = User.subscribe((e1, instance1) -> {
                    user.postValue(instance1);
                    return null;
                });

                if (error != null) {
                    ErrorRepository.captureException(error);
                    initializedFuture.completeExceptionally(error);
                    return;
                }

                user.setValue(instance);
            }));

            return initializedFuture;
        }

        return initializedFuture.thenApply(_void -> getUser().getValue());
    }

    public MutableLiveData<User> getUser() {
        return user;
    }

    @Override
    protected void onCleared() {
        super.onCleared();
        if (userStateListener != null) {
            userStateListener = null;
            AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
        }
    }

    public void signInGoogle(Activity activity, io.literal.lib.Callback<Exception, User> callback) {
        AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
        AuthenticationRepository.signInGoogle(activity, (e, userStateDetails) -> {
            try {
                if (e != null) {
                    callback.invoke(e, null);
                    return;
                }
                User.getInstance(userStateDetails).whenComplete((instance, error) -> {
                    user.postValue(instance);
                    callback.invoke((Exception) error, instance);
                });
            } catch (Exception getTokensException) {
                callback.invoke(getTokensException, null);
            } finally {
                AWSMobileClient.getInstance().addUserStateListener(userStateListener);
            }
        });
    }

    public void signUp(String email, String password, io.literal.lib.Callback<Exception, User> callback) {
        AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
        AuthenticationRepository.signUp(email, password, (e, userStateDetails) -> {
            try {
                if (e != null) {
                    callback.invoke(e, null);
                    return;
                }

                User.getInstance(userStateDetails).whenComplete((instance, error) -> {
                    user.postValue(instance);
                    callback.invoke((Exception) error, instance);
                });
            } catch (Exception getTokensException) {
                callback.invoke(getTokensException, null);
            } finally {
                AWSMobileClient.getInstance().addUserStateListener(userStateListener);
            }
        });
    }

    public void signIn(String email, String password, io.literal.lib.Callback<Exception, User> callback) {
        AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
        AuthenticationRepository.signIn(email, password, (e, userStateDetails) -> {
            if (e != null) {
                callback.invoke(e, null);
            } else {

                User.getInstance(userStateDetails).whenComplete((instance, error) -> {
                    user.postValue(instance);
                    callback.invoke((Exception) error, instance);
                });
            }
            AWSMobileClient.getInstance().addUserStateListener(userStateListener);
        });
    }

    public CompletableFuture<Void> signOut() {
        return AuthenticationRepository.signOut();
    }
}
