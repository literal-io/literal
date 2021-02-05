package io.literal.lib;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.ui.view.AppWebView;

public class WebEvent {

    // App WebView
    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";
    public static final String TYPE_ROUTER_REPLACE = "ROUTER_REPLACE";

    public static final String TYPE_AUTH_SIGN_IN = "AUTH_SIGN_IN";
    public static final String TYPE_AUTH_SIGN_IN_RESULT = "AUTH_SIGN_IN_RESULT";

    public static final String TYPE_AUTH_GET_TOKENS = "AUTH_GET_TOKENS";
    public static final String TYPE_AUTH_GET_TOKENS_RESULT = "AUTH_GET_TOKENS_RESULT";

    public static final String TYPE_AUTH_GET_USER_INFO = "AUTH_GET_USER_INFO";
    public static final String TYPE_AUTH_GET_USER_INFO_RESULT = "AUTH_GET_USER_INFO_RESULT";

    // Source WebView
    public static final String TYPE_VIEW_STATE_EDIT_ANNOTATION_TAGS = "VIEW_STATE_EDIT_ANNOTATION_TAGS";
    public static final String TYPE_EDIT_ANNOTATION_TAGS_RESULT = "EDIT_ANNOTATION_TAGS_RESULT";
    public static final String TYPE_VIEW_STATE_COLLAPSED_ANNOTATION_TAGS = "VIEW_STATE_COLLAPSED_ANNOTATION_TAGS";
    public static final String TYPE_CREATE_ANNOTATION = "CREATE_ANNOTATION";
    public static final String TYPE_FOCUS_ANNOTATION = "FOCUS_ANNOTATION";

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
        try {
            JSONObject val = new JSONObject();
            val.put("type", type);
            val.put("pid", pid);
            val.put("data", data);
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