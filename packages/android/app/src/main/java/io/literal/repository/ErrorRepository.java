package io.literal.repository;

import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateListener;

import io.literal.lib.Thunk;
import io.sentry.Sentry;
import io.sentry.protocol.User;

public class ErrorRepository {

    public static UserStateListener userStateListener = details -> {
        Log.d("ErrorRepository", "userStateListener: " + details.getUserState().name());
        if (details.getUserState().equals(UserState.SIGNED_IN)) {
            User user = new User();
            user.setUsername(AWSMobileClient.getInstance().getUsername());
            user.setId(AWSMobileClient.getInstance().getIdentityId());
            Sentry.setUser(new User());
        } else {
            Sentry.configureScope(scope -> scope.setUser(null));
        }
    };

    public static Thunk initialize() {
        AWSMobileClient.getInstance().addUserStateListener(userStateListener);
        return () -> AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
    }

    public static void captureException(Exception exception) {
        Sentry.captureException(exception);
        Log.d("ErrorRepository", "Capture Exception", exception);
    }

    public static void captureException(Exception exception, String message) {
        Sentry.configureScope(scope -> {
            scope.setContexts("message", message);
        });
        Sentry.captureException(exception);
        Sentry.configureScope(scope -> {
            scope.setContexts("message", (String) null);
        });
        Log.d("ErrorRepository", message, exception);
    }

    public static void setUser(User user) {
        Sentry.setUser(user);
    }
}
