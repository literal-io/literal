package io.literal.lib;

import android.content.Context;

import com.amazonaws.mobile.config.AWSConfiguration;

public class AWSConfigurationLib {
    private static volatile AWSConfiguration configuration;
    private static volatile AWSConfiguration guestConfiguration;

    public static AWSConfiguration getConfiguration(Context context) {
        if (configuration == null) {
            configuration = new AWSConfiguration(context);
        }
        return configuration;
    }

    public static AWSConfiguration getGuestConfiguration(Context context) {
        if (guestConfiguration == null) {
            guestConfiguration = new AWSConfiguration(context);
            guestConfiguration.setConfiguration("literal_AWS_IAM");
        }
        return guestConfiguration;
    }
}
