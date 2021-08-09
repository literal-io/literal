package io.literal.repository;

import android.app.Application;
import android.util.Log;
import org.json.JSONObject;


public class AnalyticsRepository {

    public static final String TYPE_GRAPH_QL_OPERATION = "GRAPH_QL_OPERATION";
    public static final String TYPE_ACTIVITY_START = "ACTIVITY_START";
    public static final String TYPE_DISPATCHED_WEB_EVENT = "DISPATCHED_WEB_EVENT";
    public static final String TYPE_RECEIVED_WEB_EVENT = "RECEIVED_WEB_EVENT";
    public static final String TYPE_CLICK = "CLICK";
    public static final String TYPE_HANDLE_INTENT_START = "HANDLE_INTENT_START";
    public static final String TYPE_HANDLE_INTENT_COMPLETE = "HANDLE_INTENT_COMPLETE";

    public static void initialize(Application application) {
        Log.d("AnalyticsRepository", "initialize");
    }

    public static void logEvent(String type, JSONObject data) {
        Log.d("AnalyticsRepository", "logEvent: " + type + ", " + data.toString());
    }

    public static void setUserId(String userId) {
        Log.d("AnalyticsRepository", "setUserId: " + userId);
    }
}
