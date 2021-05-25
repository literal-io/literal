package io.literal.factory;

import android.content.Context;

import com.amazonaws.auth.AWSCredentialsProvider;
import com.amazonaws.auth.CognitoCachingCredentialsProvider;
import com.amazonaws.mobile.config.AWSConfiguration;
import com.amazonaws.regions.Region;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.s3.AmazonS3Client;

import org.json.JSONObject;

import io.literal.lib.AWSConfigurationLib;

public class AmazonS3ClientFactory {
    private static volatile AmazonS3Client client;

    public static final AmazonS3Client getInstance(Context context) {
        if (client == null) {
            AWSConfiguration configuration = AWSConfigurationLib.getConfiguration(context);
            client = new AmazonS3Client(getCredentialsProvider(context, configuration));
            JSONObject s3TransferUtility = configuration.optJsonObject("S3TransferUtility");
            String region = s3TransferUtility.optString("Region");
            client.setRegion(Region.getRegion(region));
        }
        return client;
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
