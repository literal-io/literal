package io.literal.model;

import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.results.Tokens;

import org.jetbrains.annotations.NotNull;

import java.util.Map;

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
        return getState().equals(UserState.SIGNED_OUT) || getState().equals(UserState.SIGNED_OUT_FEDERATED_TOKENS_INVALID) || getState().equals(UserState.SIGNED_OUT_USER_POOLS_TOKENS_INVALID);
    }
}
