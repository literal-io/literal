package io.literal.lib;

import org.json.JSONException;
import org.json.JSONObject;

import io.literal.ui.view.WebView;

public class WebEvent {

    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";
    public static final String TYPE_ROUTER_REPLACE = "ROUTER_REPLACE";

    public static final String TYPE_AUTH_SIGN_IN = "AUTH_SIGN_IN";
    public static final String TYPE_AUTH_SIGN_IN_RESULT = "AUTH_SIGN_IN_RESULT";

    public static final String TYPE_AUTH_GET_TOKENS = "AUTH_GET_TOKENS";
    public static final String TYPE_AUTH_GET_TOKENS_RESULT = "AUTH_GET_TOKENS_RESULT";

    public static final String TYPE_AUTH_GET_USER_INFO = "AUTH_GET_USER_INFO";
    public static final String TYPE_AUTH_GET_USER_INFO_RESULT = "AUTH_GET_USER_INFO_RESULT";

    public static final String TYPE_NEW_ANNOTATION = "NEW_ANNOTATION";
    public static final String TYPE_NEW_ANNOTATION_RESULT = "NEW_ANNOTATION_RESULT";

    private String type;
    private String pid;
    private JSONObject data = new JSONObject();

    public WebEvent(JSONObject data) {
        this.type = data.optString("type");
        this.pid = data.optString("pid");
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

    public interface Callback {
        public void onWebEvent(WebView view, WebEvent event);
    }
}