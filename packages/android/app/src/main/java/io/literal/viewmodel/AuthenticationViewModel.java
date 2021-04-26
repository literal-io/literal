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
    private final UserStateListener userStateListener = details -> handleUserStateDetails(details, (_e, _user) -> { /** noop **/ });
    private ArrayList<io.literal.lib.Callback<Exception, User>> initializationCallbacks;
    private boolean hasInitialized = false;

    public void initialize(Activity activity, io.literal.lib.Callback<Exception, User> callback) {
        if (hasInitialized) {
            callback.invoke(null, getUser().getValue());
            return;
        }

        if (initializationCallbacks != null) {
            initializationCallbacks.add(callback);
            return;
        }

        initializationCallbacks = new ArrayList<>();
        initializationCallbacks.add(callback);
        AWSMobileClientFactory.initializeClient(activity.getApplicationContext(), new Callback<UserStateDetails>() {
            @Override
            public void onResult(UserStateDetails result) {
                handleUserStateDetails(result, (e, user) -> {
                    initializationCallbacks.forEach(cb -> cb.invoke(e, user));
                    initializationCallbacks = null;
                    hasInitialized = true;
                    AWSMobileClient.getInstance().addUserStateListener(userStateListener);
                });
            }

            @Override
            public void onError(Exception e) {
                ErrorRepository.captureException(e);
                initializationCallbacks.forEach(cb -> cb.invoke(e, null));
                initializationCallbacks = null;
                hasInitialized = true;
                AWSMobileClient.getInstance().addUserStateListener(userStateListener);
            }
        });
    }

    private void handleUserStateDetails(UserStateDetails userStateDetails, io.literal.lib.Callback<Exception, User> callback) {

        if (userStateDetails.getUserState().equals(UserState.SIGNED_IN)) {
            Box<Boolean> didReceiveSignedOutState = new Box(false);
            UserStateListener userStateListener = new UserStateListener() {

                @Override
                public void onUserStateChanged(UserStateDetails details) {
                    if (details.getUserState().equals(UserState.SIGNED_OUT_FEDERATED_TOKENS_INVALID) || details.getUserState().equals(UserState.SIGNED_OUT_USER_POOLS_TOKENS_INVALID) || details.getUserState().equals(UserState.SIGNED_OUT)) {
                        didReceiveSignedOutState.set(true);
                        User inst = new User(details.getUserState());
                        user.postValue(inst);
                        callback.invoke(null, inst);

                        AWSMobileClient.getInstance().releaseSignInWait();
                        AWSMobileClient.getInstance().removeUserStateListener(this);
                    }
                }
            };
            AWSMobileClient.getInstance().addUserStateListener(userStateListener);
            String identityId = AuthenticationRepository.getIdentityId();
            final String[] username = new String[1];
            final Map<String, String>[] userAttributes = new Map[1];
            final Tokens[] tokens = new Tokens[1];

            ManyCallback<Exception, Boolean> manyCallback = new ManyCallback<>(3, (e, _void) -> {
                AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
                if (didReceiveSignedOutState.get()) {
                    // userStateListener already received invalidation state
                    return;
                }

                String aliasedUsername = username[0] != null && userAttributes[0] != null
                    ? username[0].startsWith("Google") ? username[0] : userAttributes[0].get("sub")
                        : null;

                User inst = new User(
                        userStateDetails.getUserState(),
                        tokens[0],
                        aliasedUsername,
                        identityId,
                        userAttributes[0]
                );
                user.postValue(inst);
                callback.invoke(e, inst);
            });
            AuthenticationRepository.getUsername((e, data) -> {
                username[0] = data;
                manyCallback.getCallback(0).invoke(null, true);
            });
            AuthenticationRepository.getUserAttributes((e, data) -> {
                userAttributes[0] = data;
                manyCallback.getCallback(1).invoke(null, true);
            });
            AuthenticationRepository.getTokens((e, data) -> {
                tokens[0] = data;
                manyCallback.getCallback(2).invoke(null, true);
            });
        } else {
            User inst = new User(userStateDetails.getUserState());
            user.postValue(inst);
            callback.invoke(null, inst);
        }
    }

    public MutableLiveData<User> getUser() {
        return user;
    }

    public void signInGoogle(Activity activity, io.literal.lib.Callback<Exception, User> callback) {
        AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
        AuthenticationRepository.signInGoogle(activity, (e, userStateDetails) -> {
            try {
                if (e != null) {
                    callback.invoke(e, null);
                    return;
                }
                handleUserStateDetails(userStateDetails, callback);
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

                handleUserStateDetails(userStateDetails, callback);
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
                handleUserStateDetails(userStateDetails, callback);
            }
            AWSMobileClient.getInstance().addUserStateListener(userStateListener);
        });
    }
}
