package io.literal.repository;

import android.util.Log;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateListener;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Map;

import io.literal.BuildConfig;
import io.literal.lib.Thunk;
import io.literal.model.ErrorRepositoryLevel;
import io.sentry.Breadcrumb;
import io.sentry.Sentry;
import io.sentry.SentryLevel;
import io.sentry.protocol.SentryId;
import io.sentry.protocol.User;

public class ErrorRepository {

    public static String CATEGORY_AUTHENTICATION = "Authentication";
    public static String CATEGORY_NAVIGATION = "Navigation";

    public static UserStateListener userStateListener = details -> {
        if (details.getUserState().equals(UserState.SIGNED_IN)) {
            User user = new User();
            user.setUsername(AWSMobileClient.getInstance().getUsername());
            user.setId(AWSMobileClient.getInstance().getIdentityId());
            Sentry.setUser(new User());
        } else {
            Sentry.configureScope(scope -> scope.setUser(null));
        }

        Log.d("userStateListener",  details.getUserState().name());
    };

    public static Thunk initialize() {
        if (BuildConfig.DEBUG) {
            return () -> { /* noop */ };
        }
        AWSMobileClient.getInstance().addUserStateListener(userStateListener);
        return () -> AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
    }

    public static void captureException(Exception exception) {
        if (!BuildConfig.DEBUG) {
            SentryId id = Sentry.captureException(exception);

            try {
                JSONObject data = new JSONObject();
                data.put("exception", exception.getMessage());
                data.put("sentryId", id.toString());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_ERROR_REPOSITORY_EXCEPTION, data);
            } catch (JSONException e) { /** noop **/ }
        }
        Log.d("captureException", "", exception);
    }

    public static void captureException(Throwable exception, Map<String, Object> context) {
        if (!BuildConfig.DEBUG) {
            Sentry.pushScope();
            Sentry.configureScope((scope) -> context.forEach(scope::setContexts));
            SentryId id = Sentry.captureException(exception);
            Sentry.popScope();

            try {
                JSONObject data = new JSONObject();
                data.put("exception", exception.getMessage());
                data.put("sentryId", id.toString());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_ERROR_REPOSITORY_EXCEPTION, data);
            } catch (JSONException e) { /** noop **/ }
        }
        Log.d("captureException", context.toString(), exception);
    }

    public static void captureMessage(String message, ErrorRepositoryLevel level, Map<String, Object> context) {
        if (!BuildConfig.DEBUG) {
            Sentry.pushScope();
            Sentry.configureScope((scope) -> context.forEach(scope::setContexts));
            SentryId id = Sentry.captureMessage(message, SentryLevel.valueOf(level.name()));
            Sentry.popScope();

            try {
                JSONObject data = new JSONObject();
                data.put("exception", message);
                data.put("level", level.name());
                data.put("sentryId", id.toString());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_ERROR_REPOSITORY_EXCEPTION, data);
            } catch (JSONException e) { /** noop **/ }
        }
        Log.d("captureMessage", message + ", " + context.toString());
    }

    public static void captureException(Throwable exception) {
        if (!BuildConfig.DEBUG) {
            Sentry.captureException(exception);
        }
        Log.d("captureException", "", exception);
    }

    public static void captureException(Exception exception, String message) {
        if (!BuildConfig.DEBUG) {
            Sentry.configureScope(scope -> {
                scope.setContexts("message", message);
            });
            SentryId id = Sentry.captureException(exception);
            Sentry.configureScope(scope -> {
                scope.setContexts("message", (String) null);
            });

            try {
                JSONObject data = new JSONObject();
                data.put("exception", exception.getMessage());
                data.put("sentryId", id.toString());
                data.put("message", message);
            } catch (JSONException e) { /** noop **/ }
        }
        Log.d("captureException", message, exception);
    }

    public static void captureWarning(Exception exception) {
        Log.d("captureWarning", "", exception);
    }

    public static void captureBreadcrumb(String category, String message, ErrorRepositoryLevel level) {
        if (!BuildConfig.DEBUG) {
            Breadcrumb breadcrumb = new Breadcrumb();
            breadcrumb.setCategory(category);
            breadcrumb.setMessage(message);
            breadcrumb.setLevel(SentryLevel.valueOf(level.name()));
            Sentry.addBreadcrumb(breadcrumb);

            try {
                JSONObject data = new JSONObject();
                data.put("category", category);
                data.put("message", message);
                data.put("level", level.name());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_ERROR_REPOSITORY_BREADCRUMB, data);
            } catch (JSONException e) { /** noop **/ }
        }
        Log.d("captureBreadcrumb", category + ", " + message);
    }
}
