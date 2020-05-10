package io.literal.factory;

import android.content.Context;

import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.auth.CognitoCachingCredentialsProvider;
import com.amazonaws.mobile.config.AWSConfiguration;
import com.amazonaws.mobileconnectors.appsync.AWSAppSyncClient;
import com.amazonaws.mobileconnectors.appsync.S3ObjectManagerImplementation;
import com.amazonaws.regions.Region;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.s3.AmazonS3Client;

import org.json.JSONObject;

public class AppSyncClientFactory {

    private static volatile AWSConfiguration configuration;
    private static volatile AWSAppSyncClient client;
    private static volatile S3ObjectManagerImplementation s3ObjectManager;

    public static AWSAppSyncClient getInstance(Context context) {
        // FIXME: Authenticate API access, get guest AwsCredentials
        if (client == null) {
            client = AWSAppSyncClient.builder()
                    .context(context)
                    .awsConfiguration(getConfiguration(context))
                    .s3ObjectManager(getS3ObjectManager(context, getConfiguration(context)))
                    .credentialsProvider(getCredentialsProvider(context, getConfiguration(context)))
                    .build();
        }
        return client;
    }

    public static AWSConfiguration getConfiguration(Context context) {
        if (configuration == null) {
            configuration = new AWSConfiguration(context);
        }
        return configuration;
    }

    public static final S3ObjectManagerImplementation getS3ObjectManager(
            final Context context,
            final AWSConfiguration configuration
    ) {
        if (s3ObjectManager == null) {

            JSONObject s3TransferUtility = configuration.optJsonObject("S3TransferUtility");
            String region = s3TransferUtility.optString("Region");
            AmazonS3Client s3Client = new AmazonS3Client(getCredentialsProvider(context, configuration));
            s3Client.setRegion(Region.getRegion(region));
            s3ObjectManager = new S3ObjectManagerImplementation(s3Client);
        }
        return s3ObjectManager;
    }

    // initialize and fetch cognito credentials provider for S3 Object Manager
    public static final AWSCredentialsProvider getCredentialsProvider(final Context context, final AWSConfiguration configuration) {

        JSONObject credentialsProviderJson = configuration.optJsonObject("CredentialsProvider");
        if (credentialsProviderJson == null) {
            return null;
        }
        JSONObject cognitoIdentityJson = credentialsProviderJson.optJSONObject("CognitoIdentity");
        if (cognitoIdentityJson == null) {
            return null;
        }
        JSONObject defaultJson = cognitoIdentityJson.optJSONObject("Default");
        if (defaultJson == null) {
            return null;
        }

        String poolId = defaultJson.optString("PoolId");
        String region = defaultJson.optString("Region");

        final CognitoCachingCredentialsProvider credentialsProvider = new CognitoCachingCredentialsProvider(
                context,
                poolId,
                Regions.fromName(region)
        );
        return credentialsProvider;
    }
}
