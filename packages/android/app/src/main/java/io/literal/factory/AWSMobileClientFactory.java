package io.literal.factory;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.config.AWSConfiguration;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferService;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferUtility;
import com.amazonaws.services.s3.AmazonS3Client;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CountDownLatch;

import io.literal.R;
import io.literal.lib.Constants;
import io.literal.repository.ErrorRepository;
import io.literal.repository.SharedPreferencesRepository;

public class AWSMobileClientFactory {

    public enum AmplifyEnvironment {
        STAGING,
        PRODUCTION
    }

    public static volatile TransferUtility transferUtility;
    public static volatile AmplifyEnvironment amplifyEnvironment;
    private static volatile AmazonS3Client amazonS3Client;

    private static CompletableFuture<Void> initializedFuture;

    public static CompletableFuture<UserStateDetails> initializeClient(Context context, boolean forceReinitialize) {
        boolean isSignedOut = SharedPreferencesRepository.getIsSignedOut(context);
        if (initializedFuture == null || forceReinitialize) {
            initializedFuture = new CompletableFuture<>();
            try {
                context.startService(new Intent(context, TransferService.class));
            } catch (Exception ex) { /** May be in background, noop **/}
            AWSMobileClient.getInstance().initialize(context, getConfiguration(context), new Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    initializedFuture.complete(null);
                }

                @Override
                public void onError(Exception e) {
                    initializedFuture.completeExceptionally(e);
                }
            });
        }

        return initializedFuture.thenCompose((_void) -> {
            CompletableFuture<UserStateDetails> userStateDetailsFuture = new CompletableFuture<>();
            AWSMobileClient.getInstance().currentUserState(new Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    userStateDetailsFuture.complete(result);
                }

                @Override
                public void onError(Exception e) {
                    userStateDetailsFuture.completeExceptionally(e);
                }
            });
            return userStateDetailsFuture;
        }).thenCompose((userStateDetails) -> {
            if (userStateDetails.getUserState().equals(UserState.SIGNED_OUT) && !forceReinitialize && !isSignedOut) {
                AWSMobileClient.getInstance().getCredentials();
                return initializeClient(context, true);
            }
            return CompletableFuture.completedFuture(userStateDetails);
        });
    }

    public static CompletableFuture<UserStateDetails> initializeClient(Context context) {
        return initializeClient(context, false);
    }

    public static AmplifyEnvironment getAmplifyEnvironment() { return amplifyEnvironment; }

    public static AmazonS3Client getAmazonS3Client() {
        if (amazonS3Client == null) {
            amazonS3Client = new AmazonS3Client(AWSMobileClient.getInstance());
        }
        return amazonS3Client;
    }

    public static TransferUtility getTransferUtility(Context context) {
        if (transferUtility == null) {
            AWSMobileClient mobileClient = AWSMobileClient.getInstance();
            transferUtility = TransferUtility
                    .builder()
                    .context(context)
                    .awsConfiguration(mobileClient.getConfiguration())
                    .s3Client(getAmazonS3Client())
                    .build();
        }
        return transferUtility;
    }

    private static JSONObject parseInputStream(InputStream inputStream) {
        try {
            byte[] bytes = new byte[inputStream.available()];
            inputStream.read(bytes, 0, bytes.length);
            String json = new String(bytes);
            inputStream.close();
            return new JSONObject(json);
        } catch (IOException e) {
            return null;
        } catch (JSONException e) {
            return null;
        }
    }

    private static String parseUrisToProtocol (String input) {
        String[] uris = input.split(",");
        String outputUri = null;
        for (int idx = 0; idx < uris.length; idx++) {
            if (uris[idx].startsWith("literal://")) {
                outputUri = uris[idx];
            }
        }
        return outputUri;
    }

    private static AWSConfiguration getConfiguration(Context context) {
        InputStream inputStream = context.getResources().openRawResource(R.raw.awsconfiguration);
        JSONObject configuration = parseInputStream(inputStream);
        if (configuration == null) {
            return new AWSConfiguration(context);
        }

        Optional<JSONObject> oauthJson = Optional.ofNullable(configuration.optJSONObject("Auth"))
                .flatMap((json) -> Optional.ofNullable(json.optJSONObject("Default")))
                .flatMap((json) -> Optional.ofNullable(json.optJSONObject("OAuth")));

        // Prune OAuth URIs used by other environments, we only care about the literal:// protocol.
        oauthJson.ifPresent((json) -> {
            try {
                String signInRedirectURIs = json.getString("SignInRedirectURI");
                String signOutRedirectURIs = json.getString("SignOutRedirectURI");
                json.put("SignInRedirectURI", parseUrisToProtocol(signInRedirectURIs));
                json.put("SignOutRedirectURI", parseUrisToProtocol(signOutRedirectURIs));
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            }
        });

        amplifyEnvironment = Optional.ofNullable(configuration.optJSONObject("AppSync"))
                .flatMap((json) -> Optional.ofNullable(json.optJSONObject("Default")))
                .flatMap((json) -> Optional.ofNullable(json.optString("ApiUrl")))
                .map((apiUrl) ->
                        apiUrl.contains("bmxw7qjae5bwrh2ponhutmos6e")
                                ? AmplifyEnvironment.STAGING
                                : AmplifyEnvironment.PRODUCTION
                )
                .orElse(AmplifyEnvironment.STAGING);

        return new AWSConfiguration(configuration);
    }
}
