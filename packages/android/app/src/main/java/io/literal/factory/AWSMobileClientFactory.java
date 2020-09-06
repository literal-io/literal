package io.literal.factory;

import android.content.Context;
import android.content.Intent;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.config.AWSConfiguration;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferService;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferUtility;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.CountDownLatch;

import io.literal.R;

public class AWSMobileClientFactory {

    public static volatile TransferUtility transferUtility;

    static CountDownLatch initializationLatch = new CountDownLatch(1);

    public static void initializeClient(Context context, final Callback<UserStateDetails> callback) {
        context.startService(new Intent(context, TransferService.class));
        AWSMobileClient.getInstance().initialize(context, getConfiguration(context), new Callback<UserStateDetails>() {
            @Override
            public void onResult(UserStateDetails result) {
                if (callback != null) {
                    callback.onResult(result);
                }
                initializationLatch.countDown();
            }

            @Override
            public void onError(Exception e) {
                if (callback != null) {
                    callback.onError(e);
                }
            }
        });
    }

    public static void initializeClient(Context context) { initializeClient(context, null); }

    public static void initializeClientBlocking(Context context) throws InterruptedException {
        initializeClient(context, null);
        initializationLatch.await();
    }

    public static TransferUtility getTransferUtility(Context context) {
        if (transferUtility == null) {
            AWSMobileClient mobileClient = AWSMobileClient.getInstance();
            transferUtility = TransferUtility
                    .builder()
                    .context(context)
                    .awsConfiguration(mobileClient.getConfiguration())
                    .s3Client(new AmazonS3Client(mobileClient))
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
        if (configuration == null) { return new AWSConfiguration(context); }

        try {
            JSONObject authJson = configuration.getJSONObject("Auth");
            JSONObject defaultJson = authJson.getJSONObject("Default");
            JSONObject oauthJson = defaultJson.getJSONObject("OAuth");

            String signInRedirectURIs = oauthJson.getString("SignInRedirectURI");
            String signOutRedirectURIs = oauthJson.getString("SignOutRedirectURI");

            oauthJson.put("SignInRedirectURI", parseUrisToProtocol(signInRedirectURIs));
            oauthJson.put("SignOutRedirectURI", parseUrisToProtocol(signOutRedirectURIs));
        } catch (JSONException ex) {
            return new AWSConfiguration(configuration);
        }

        return new AWSConfiguration(configuration);
    }
}
