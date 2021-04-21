package io.literal.lib;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.ui.view.AppWebView;

public class WebEvent {

    // App WebView
    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";
    public static final String TYPE_ROUTER_REPLACE = "ROUTER_REPLACE";
    public static final String TYPE_AUTH_SIGN_UP = "AUTH_SIGN_UP";
    public static final String TYPE_AUTH_SIGN_UP_RESULT = "AUTH_SIGN_UP_RESULT";
    public static final String TYPE_AUTH_SIGN_IN = "AUTH_SIGN_IN";
    public static final String TYPE_AUTH_SIGN_IN_RESULT = "AUTH_SIGN_IN_RESULT";
    public static final String TYPE_AUTH_SIGN_IN_GOOGLE = "AUTH_SIGN_IN_GOOGLE";
    public static final String TYPE_AUTH_SIGN_IN_GOOGLE_RESULT = "AUTH_SIGN_IN_GOOGLE_RESULT";
    public static final String TYPE_AUTH_GET_TOKENS = "AUTH_GET_TOKENS";
    public static final String TYPE_AUTH_GET_TOKENS_RESULT = "AUTH_GET_TOKENS_RESULT";
    public static final String TYPE_AUTH_GET_USER_INFO = "AUTH_GET_USER_INFO";
    public static final String TYPE_AUTH_GET_USER_INFO_RESULT = "AUTH_GET_USER_INFO_RESULT";
    public static final String TYPE_VIEW_TARGET_FOR_ANNOTATION = "VIEW_TARGET_FOR_ANNOTATION";
    public static final String TYPE_SET_CACHE_ANNOTATION = "SET_CACHE_ANNOTATION";
    public static final String TYPE_DELETE_CACHE_ANNOTATION = "DELETE_CACHE_ANNOTATION";
    public static final String TYPE_CREATE_ANNOTATION_FROM_SOURCE = "CREATE_ANNOTATION_FROM_SOURCE";
    public static final String TYPE_ADD_CACHE_ANNOTATIONS = "ADD_CACHE_ANNOTATIONS";
    public static final String TYPE_ANALYTICS_LOG_EVENT = "ANALYTICS_LOG_EVENT";
    public static final String TYPE_ANALYTICS_SET_USER_ID = "ANALYTICS_SET_USER_ID";

    // Source WebView
    public static final String TYPE_VIEW_STATE_EDIT_ANNOTATION_TAGS = "VIEW_STATE_EDIT_ANNOTATION_TAGS";
    public static final String TYPE_EDIT_ANNOTATION_TAGS_RESULT = "EDIT_ANNOTATION_TAGS_RESULT";
    public static final String TYPE_VIEW_STATE_COLLAPSED_ANNOTATION_TAGS = "VIEW_STATE_COLLAPSED_ANNOTATION_TAGS";
    public static final String TYPE_SET_VIEW_STATE = "SET_VIEW_STATE";
    public static final String TYPE_RENDER_ANNOTATIONS = "RENDER_ANNOTATIONS";
    public static final String TYPE_CREATE_ANNOTATION = "CREATE_ANNOTATION";
    public static final String TYPE_FOCUS_ANNOTATION = "FOCUS_ANNOTATION";
    public static final String TYPE_BLUR_ANNOTATION = "BLUR_ANNOTATION";
    public static final String TYPE_EDIT_ANNOTATION = "EDIT_ANNOTATION";
    public static final String TYPE_SELECTION_CREATED = "SELECTION_CREATED";
    public static final String TYPE_ANNOTATION_RENDERER_INITIALIZED = "ANNOTATION_RENDERER_INITIALIZED";
    public static final String TYPE_SELECTION_CHANGE = "SELECTION_CHANGE";

    private String type;
    private String pid;
    private JSONObject data = new JSONObject();

    public WebEvent(JSONObject data) {
        this.type = data.optString("type");
        this.pid = data.optString("pid");
        this.data = data.optJSONObject("data");
    }

    public WebEvent(String type, String pid, JSONObject data) {
        this.type = type;
        this.pid = pid;
        this.data = data;
    }

    public JSONObject toJSON() {
        return this.toJSON(true);
    }

    public JSONObject toJSON(boolean includeData) {
        try {
            JSONObject val = new JSONObject();
            val.put("type", type);
            val.put("pid", pid);
            if (includeData) {
                val.put("data", data);
            }
            return val;
        } catch (JSONException ex) {
            return null;
        }
    }

    public String getType() {
        return type;
    }
    public JSONObject getData() {
        return data;
    }

    public interface Callback {
        public void onWebEvent(AppWebView view, WebEvent event);
    }
}