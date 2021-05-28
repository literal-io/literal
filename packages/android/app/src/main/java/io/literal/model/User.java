package io.literal.model;

import com.amazonaws.auth.AWSCredentials;
import com.amazonaws.auth.AWSSessionCredentials;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.UserStateListener;
import com.amazonaws.mobile.client.results.Tokens;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.Collections;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;

import io.literal.repository.AuthenticationRepository;
import io.literal.repository.ErrorRepository;
import kotlin.jvm.functions.Function2;

public class User {
    private final Credentials credentials;
    private final String identityId;
    private final String username;
    private final UserState state;
    private final Map<String, String> attributes;

    public User(@NotNull UserState state, Credentials credentials, String username, String identityId, Map<String, String> attributes) {
        this.state = state;
        this.credentials = credentials;
        this.identityId = identityId;
        this.username = username;
        this.attributes = attributes;
    }

    public User(@NotNull UserState state) {
        this(state, null, null, null, null);
    }

    public static UserStateListener subscribe(Function2<Exception, User, Void> callback) {
        return details -> getInstance(details).whenComplete((user, error) -> callback.invoke(null, user));
    }

    public static CompletableFuture<User> getInstance(UserStateDetails userStateDetails) {
        CompletableFuture<User> userFuture = new CompletableFuture<>();
        if (userStateDetails.getUserState().equals(UserState.SIGNED_IN) || userStateDetails.getUserState().equals(UserState.GUEST)) {
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

            CompletableFuture<String> identityIdFuture;
            CompletableFuture<String> usernameFuture;
            CompletableFuture<Map<String, String>> userAttributesFuture;
            CompletableFuture<Credentials> credentialsFuture;

            if (userStateDetails.getUserState().equals(UserState.SIGNED_IN)) {
                identityIdFuture = AuthenticationRepository.getIdentityIdWithRetry();
                userAttributesFuture = AuthenticationRepository.getUserAttributesWithRetry();
                CompletableFuture<Tokens> tokensFuture = AuthenticationRepository.getTokensWithRetry();
                credentialsFuture = CompletableFuture.allOf(tokensFuture, identityIdFuture).thenCompose(_void -> {
                    CompletableFuture<Credentials> innerCredentialsFuture = new CompletableFuture<>();
                    try {
                        innerCredentialsFuture.complete(new Credentials(tokensFuture.get(), identityIdFuture.get()));
                    } catch (Exception e) {
                        innerCredentialsFuture.completeExceptionally(e);
                    }
                    return innerCredentialsFuture;
                });

                CompletableFuture<String> rawUsernameFuture = AuthenticationRepository.getUsernameWithRetry();
                usernameFuture = CompletableFuture.allOf(userAttributesFuture, rawUsernameFuture)
                        .thenCompose(_void -> {
                            CompletableFuture<String> aliasedUsernameFuture = new CompletableFuture<String>();
                            try {
                                String username = rawUsernameFuture.get();
                                Map<String, String> userAttributes = userAttributesFuture.get();

                                if (username == null || userAttributes == null) {
                                    aliasedUsernameFuture.completeExceptionally(new AuthenticationRepository.InvalidStateException("Username is null."));
                                    return aliasedUsernameFuture;
                                }
                                aliasedUsernameFuture.complete(username.startsWith("Google") ? username : userAttributes.get("sub"));
                            } catch (Exception ex) {
                                aliasedUsernameFuture.completeExceptionally(ex);
                            }

                            return aliasedUsernameFuture;
                        });
            } else {
                identityIdFuture = AuthenticationRepository.getIdentityIdWithRetry();
                usernameFuture = AuthenticationRepository.getUsernameWithRetry();
                userAttributesFuture = CompletableFuture.completedFuture(Collections.emptyMap());
                credentialsFuture = identityIdFuture.thenApply((identityId) -> new Credentials((AWSSessionCredentials) AWSMobileClient.getInstance().getCredentials(), identityId));
            }

            CompletableFuture.allOf(identityIdFuture, usernameFuture, userAttributesFuture, credentialsFuture)
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
                            Credentials credentials = credentialsFuture.get();

                            userFuture.complete(new User(
                                    userStateDetails.getUserState(),
                                    credentials,
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

    public Map<String, String> getAttributes() {
        return attributes;
    }

    public UserState getState() {
        return state;
    }

    public String getIdentityId() {
        return identityId;
    }

    public String getEncodedIdentityId() {
        String encodedIdentityId = identityId;

        if (identityId != null) {
            try {
                encodedIdentityId = URLEncoder.encode(identityId, "UTF-8");
            } catch (UnsupportedEncodingException e) {
                ErrorRepository.captureException(e);
            }
        }
        return encodedIdentityId;
    }

    /**
     * When signed in, we don't want to use the identityId as the appsync identity (i.e. creatorUsername),
     * as its not encoded within the user pool JWT. When using guest authentication, the cognito identityId
     * is available and is the only stable ID, so use that. Confusingly, we should use the cognito identity ID
     * for keying S3 objects regardless of authentication state.
     * @return
     */
    public String getAppSyncIdentity() {
        if (state.equals(UserState.SIGNED_IN)) {
            return username;
        }
        return identityId;
    }

    public Credentials getCredentials() {
        return credentials;
    }

    public String getUsername() {
        return username;
    }

    public boolean isSignedOut() {
        return !getState().equals(UserState.SIGNED_IN);
    }

    public JSONObject toJSON() {
        try {
            JSONObject result = new JSONObject();
            result.put("username", username);
            result.put("identityId", identityId);
            result.put("state", state.name());
            if (credentials != null) {
                result.put("credentials", credentials.toJSON());
            }
            if (attributes != null && !attributes.isEmpty()) {
                result.put("attributes", new JSONObject(attributes));
            }
            return result;
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
            return null;
        }
    }

    public static class Credentials {
        private final Tokens tokens;
        private final AWSSessionCredentials awsCredentials;
        private final String identityId;

        public Credentials(Tokens tokens, String identityId) {
            this.tokens = tokens;
            this.identityId = identityId;
            this.awsCredentials = null;
        }

        public Credentials(AWSSessionCredentials awsCredentials, String identityId) {
            this.awsCredentials = awsCredentials;
            this.identityId = identityId;
            this.tokens = null;
        }

        public Tokens getTokens() {
            return tokens;
        }

        public JSONObject toJSON() {
            try {
                JSONObject output = new JSONObject();
                if (tokens != null) {
                    JSONObject tokensJSON = new JSONObject();
                    tokensJSON.put("idToken", tokens.getIdToken().getTokenString());
                    tokensJSON.put("refreshToken", tokens.getRefreshToken().getTokenString());
                    tokensJSON.put("accessToken", tokens.getAccessToken().getTokenString());
                    output.put("tokens", tokensJSON);
                } else {
                    JSONObject credentialsJSON = new JSONObject();
                    credentialsJSON.put("accessKeyId", awsCredentials.getAWSAccessKeyId());
                    credentialsJSON.put("secretAccessKey", awsCredentials.getAWSSecretKey());
                    credentialsJSON.put("sessionToken", awsCredentials.getSessionToken());
                    credentialsJSON.put("identityId", identityId);
                    output.put("awsCredentials", credentialsJSON);
                }

                return output;
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
                return null;
            }
        }
    }
}
