package io.literal.factory;

import android.content.Context;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.config.AWSConfiguration;
import com.google.gson.JsonObject;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;

import io.literal.R;

public class AWSMobileClientFactory {

    private static volatile boolean hasInitialized = false;

    public static AWSMobileClient getInstance(Context context) {

        if (!hasInitialized) {
            AWSMobileClient.getInstance().initialize(context, getConfiguration(context), new Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    hasInitialized = true;
                }

                @Override
                public void onError(Exception e) {

                }
            });

        }
        return AWSMobileClient.getInstance();
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
