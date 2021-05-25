package io.literal.factory;

import android.app.AuthenticationRequiredException;
import android.content.Context;
import android.util.Log;

import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.auth.CognitoCachingCredentialsProvider;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.results.Tokens;
import com.amazonaws.mobile.config.AWSConfiguration;
import com.amazonaws.mobileconnectors.appsync.AWSAppSyncClient;
import com.amazonaws.mobileconnectors.appsync.S3ObjectManagerImplementation;
import com.amazonaws.regions.Region;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.s3.AmazonS3Client;

import org.json.JSONObject;

import java.util.concurrent.TimeUnit;

import io.literal.lib.AWSConfigurationLib;
import io.literal.lib.Constants;
import io.literal.lib.WebRoutes;
import io.literal.model.User;
import io.literal.repository.AuthenticationRepository;
import io.literal.repository.ErrorRepository;
import okhttp3.OkHttpClient;
import okhttp3.Request;

public class AppSyncClientFactory {

    private static volatile AWSConfiguration configuration;
    private static volatile AmazonS3Client s3Client;
    private static volatile AWSAppSyncClient authenticatedClient;
    private static volatile AWSAppSyncClient guestClient;

    public static AWSAppSyncClient getInstanceForUser(Context context, User user) {
        if (user.getState().equals(UserState.SIGNED_IN)) {
            return getAuthenticatedInstanceForUser(context, user);
        }
        return getGuestInstance(context);
    }

    private static AWSAppSyncClient.Builder getBaseInstanceBuilder(Context context) {
        return AWSAppSyncClient.builder()
                .context(context)
                .okHttpClient(
                        new OkHttpClient.Builder()
                                .addNetworkInterceptor(
                                        chain -> {
                                            Request newRequest = chain.request().newBuilder()
                                                    .addHeader("origin", WebRoutes.getAPIHost())
                                                    .build();
                                            return chain.proceed(newRequest);
                                        }
                                )
                                .readTimeout(30, TimeUnit.SECONDS)
                                .writeTimeout(30, TimeUnit.SECONDS)
                                .build()
                );
    }

    private static AWSAppSyncClient getAuthenticatedInstanceForUser(Context context, User user) {
        if (authenticatedClient == null) {
            authenticatedClient = getBaseInstanceBuilder(context)
                    .cognitoUserPoolsAuthProvider(() -> {
                        Tokens tokens = user.getCredentials().getTokens();
                        if (tokens == null) {
                            ErrorRepository.captureException(new AuthenticationRepository.InvalidStateException("Expected valid credentials for authenticated client, but found none."));
                            return null;
                        }
                        return tokens.getIdToken().getTokenString();
                    })
                    .awsConfiguration(AWSConfigurationLib.getConfiguration(context))
                    .build();
        }
        return authenticatedClient;
    }

    private static AWSAppSyncClient getGuestInstance(Context context) {
        if (guestClient == null) {
            guestClient = getBaseInstanceBuilder(context)
                    .credentialsProvider(AWSMobileClient.getInstance())
                    .awsConfiguration(AWSConfigurationLib.getGuestConfiguration(context))
                    .build();
        }
        return guestClient;
    }
}
