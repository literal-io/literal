package io.literal.model;

import android.app.VoiceInteractor;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.UserStateListener;
import com.amazonaws.mobile.client.results.Token;
import com.amazonaws.mobile.client.results.Tokens;

import org.jetbrains.annotations.NotNull;

import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.function.BiFunction;

import io.literal.lib.Box;
import io.literal.lib.Callback;
import io.literal.lib.ManyCallback;
import io.literal.repository.AuthenticationRepository;
import io.literal.repository.ErrorRepository;
import kotlin.jvm.functions.Function1;
import kotlin.jvm.functions.Function2;

public class User {
    private final Tokens tokens;
    private final String identityId;
    private final String username;
    private final UserState state;
    private final Map<String, String> attributes;

    public User(@NotNull UserState state, Tokens tokens, String username, String identityId, Map<String, String> attributes) {
        this.state = state;
        this.tokens = tokens;
        this.identityId = identityId;
        this.username = username;
        this.attributes = attributes;
    }

    public User(@NotNull UserState state) {
        this(state, null, null, null, null);
    }

    public Map<String, String> getAttributes() {
        return attributes;
    }

    public UserState getState() {
        return state;
    }

    public String getIdentityId() {
        return identityId;
    }

    public Tokens getTokens() {
        return tokens;
    }

    public String getUsername() {
        return username;
    }

    public boolean isSignedOut() {
        return !getState().equals(UserState.SIGNED_IN);
    }

    public static UserStateListener subscribe(Function2<Exception, User, Void> callback) {
        return details -> getInstance(details).whenComplete((user, error) -> callback.invoke(null, user));
    }

    public static CompletableFuture<User> getInstance(UserStateDetails userStateDetails) {
        CompletableFuture<User> userFuture = new CompletableFuture<>();
        if (userStateDetails.getUserState().equals(UserState.SIGNED_IN)) {
            UserStateListener userStateListener = new UserStateListener() {
                @Override
                public void onUserStateChanged(UserStateDetails details) {
                    if (details.getUserState().equals(UserState.SIGNED_OUT_FEDERATED_TOKENS_INVALID) || details.getUserState().equals(UserState.SIGNED_OUT_USER_POOLS_TOKENS_INVALID) || details.getUserState().equals(UserState.SIGNED_OUT)) {
                        userFuture.complete(new User(details.getUserState()));
                        AWSMobileClient.getInstance().releaseSignInWait();
                        AWSMobileClient.getInstance().removeUserStateListener(this);
                    }
                }
            };
            AWSMobileClient.getInstance().addUserStateListener(userStateListener);
            CompletableFuture<String> identityIdFuture = AuthenticationRepository.getIdentityIdWithRetry();
            CompletableFuture<String> usernameFuture = AuthenticationRepository.getUsernameWithRetry();
            CompletableFuture<Map<String, String>> userAttributesFuture = AuthenticationRepository.getUserAttributesWithRetry();
            CompletableFuture<Tokens> tokensFuture = AuthenticationRepository.getTokensWithRetry();

            CompletableFuture.allOf(identityIdFuture, usernameFuture, userAttributesFuture, tokensFuture)
                    .whenComplete((_void, error) -> {
                        if (userFuture.isDone()) {
                            return;
                        }

                        if (error != null) {
                            ErrorRepository.captureException(error);
                            userFuture.complete(new User(UserState.UNKNOWN));
                        }

                        try {
                            String identityId = identityIdFuture.get();
                            String username = usernameFuture.get();
                            Map<String, String> userAttributes = userAttributesFuture.get();
                            Tokens tokens = tokensFuture.get();

                            userFuture.complete(new User(
                                    userStateDetails.getUserState(),
                                    tokens,
                                    username,
                                    identityId,
                                    userAttributes
                            ));
                        } catch (Exception e) {
                            ErrorRepository.captureException(e);
                            userFuture.complete(new User(UserState.UNKNOWN));
                        }
                    });
        } else {
            userFuture.complete(new User(userStateDetails.getUserState()));
        }

        return userFuture;
    }
}
