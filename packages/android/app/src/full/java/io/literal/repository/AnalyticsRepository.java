package io.literal.repository;

import android.app.Application;
import android.util.Log;

import com.amplitude.api.Amplitude;

import org.json.JSONObject;

import io.literal.BuildConfig;

public class AnalyticsRepository {

    private static final String AMPLITUDE_API_KEY = "8f1701d791829e62f64f1c680c3f78d1";

    public static final String TYPE_GRAPH_QL_OPERATION = "GRAPH_QL_OPERATION";
    public static final String TYPE_ACTIVITY_START = "ACTIVITY_START";
    public static final String TYPE_DISPATCHED_WEB_EVENT = "DISPATCHED_WEB_EVENT";
    public static final String TYPE_RECEIVED_WEB_EVENT = "RECEIVED_WEB_EVENT";
    public static final String TYPE_CLICK = "CLICK";
    public static final String TYPE_DISPATCH_SERVICE_INTENT = "DISPATCH_SERVICE_INTENT";
    public static final String TYPE_HANDLE_INTENT_START = "HANDLE_INTENT_START";
    public static final String TYPE_HANDLE_INTENT_COMPLETE = "HANDLE_INTENT_COMPLETE";
    public static final String TYPE_ERROR_REPOSITORY_BREADCRUMB = "ERROR_REPOSITORY_BREADCRUMB";
    public static final String TYPE_ERROR_REPOSITORY_EXCEPTION = "ERROR_REPOSITORY_EXCEPTION";
    public static final String TYPE_ERROR_REPOSITORY_WARNING = "ERROR_REPOSITORY_WARNING";
    public static final String TYPE_ANNOTATION_RENDERER_FAILED_TO_INITIALIZE = "ANNOTATION_RENDERER_FAILED_TO_INITIALIZE";
    public static final String TYPE_ANNOTATION_RENDERER_INITIALIZED = "ANNOTATION_RENDERER_INITIALIZED";

    public static void initialize(Application application) {
        Amplitude.getInstance().initialize(application.getApplicationContext(), AMPLITUDE_API_KEY)
                .enableForegroundTracking(application);

        if (BuildConfig.DEBUG) {
            Log.d("AnalyticsRepository", "initialize");
        }
    }

    public static void logEvent(String type, JSONObject data) {
        Amplitude.getInstance().logEvent(type, data);
        if (BuildConfig.DEBUG) {
            Log.d("AnalyticsRepository", "logEvent: " + type + ", " + data.toString());
        }
    }

    public static void setUserId(String userId) {
        Amplitude.getInstance().setUserId(userId);
        if (BuildConfig.DEBUG) {
            Log.d("AnalyticsRepository", "setUserId: " + userId);
        }
    }
}
