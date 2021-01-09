package io.literal.viewmodel;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.results.Tokens;

import java.util.Map;
import java.util.concurrent.CountDownLatch;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.repository.AuthenticationRepository;

public class AuthenticationViewModel extends ViewModel {


    private final MutableLiveData<UserStateDetails> userStateDetails = new MutableLiveData<>(null);
    private final MutableLiveData<Tokens> tokens = new MutableLiveData<>(null);
    private final MutableLiveData<String> username = new MutableLiveData<>(null);
    private final MutableLiveData<String> identityId = new MutableLiveData<>(null);
    private final MutableLiveData<Map<String, String>> userAttributes = new MutableLiveData<>(null);
    private final CountDownLatch hasInitializedLatch = new CountDownLatch(1);

    public MutableLiveData<UserStateDetails> getUserStateDetails() {
        return userStateDetails;
    }
    public MutableLiveData<Tokens> getTokens() {
        if (tokens.getValue() == null) {
            try {
                tokens.setValue(AuthenticationRepository.getTokens());
            } catch (Exception e) {
                Log.d("AuthenticationViewModel", "getTokens", e);
            }
        }
        return tokens;
    }

    public MutableLiveData<Map<String, String>> getUserAttributes() {
        if (userAttributes.getValue() == null) {
            try {
                userAttributes.setValue(AuthenticationRepository.getUserAttributes());
            } catch (Exception e) {
                Log.d("AuthenticationViewModel", "getUserInfo", e);
            }
        }
        return userAttributes;
    }

    public MutableLiveData<String> getUsername() {
        if (username.getValue() == null) {
            username.setValue(AuthenticationRepository.getUsername());
        }
        return username;
    }

    public MutableLiveData<String> getIdentityId() {
        if (identityId.getValue() == null) {
            identityId.setValue(AuthenticationRepository.getUsername());
        }
        return identityId;
    }

    public void initialize(Context context) {
        AWSMobileClientFactory.initializeClient(context, new Callback<UserStateDetails>() {
            @Override
            public void onResult(UserStateDetails result) {
                userStateDetails.postValue(result);
                hasInitializedLatch.countDown();
                asyncInitializeValues();
            }

            @Override
            public void onError(Exception e) {
                Log.d("AuthenticationViewModel", "initialize", e);
            }
        });
    }

    private void asyncInitializeValues() {
        tokens.postValue(null);
        AuthenticationRepository.getTokens((e, tokensResult) -> {
            tokens.postValue(tokensResult);
        });
        userAttributes.postValue(null);
        AuthenticationRepository.getUserAttributes((e, userInfoResult) -> {
            userAttributes.postValue(userInfoResult);
        });
        identityId.postValue(AuthenticationRepository.getIdentityId());
        username.postValue(AuthenticationRepository.getUsername());
    }

    public void awaitInitialization() {
        try {
            hasInitializedLatch.await();
        } catch (InterruptedException e) {
            Log.d("AuthenticationViewModel", "awaitInitialization", e);
        }
    }

    public void signInGoogle(Activity activity, AuthenticationRepository.Callback<Void> callback) {
        AuthenticationRepository.signInGoogle(activity, (e, userStateDetails) -> {
            try {
                if (e != null) {
                    callback.invoke(e, null);
                    return;
                }
                this.userStateDetails.postValue(userStateDetails);
                this.tokens.postValue(AuthenticationRepository.getTokens());
                this.userAttributes.postValue(AuthenticationRepository.getUserAttributes());
                callback.invoke(null, null);
            } catch (Exception getTokensException) {
                callback.invoke(getTokensException, null);
            }
        });
    }
}
