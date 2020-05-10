package io.literal.lib;

import org.json.JSONException;
import org.json.JSONObject;

public class WebEvent {

    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";
    public static final String TYPE_ROUTER_REPLACE = "ROUTER_REPLACE";

    private String type;
    private String pid;
    private JSONObject data = new JSONObject();

    public WebEvent (JSONObject data) {
        this.type = data.optString("type");
        this.pid = data.optString("pid");
    }

    public WebEvent (String type, String pid, JSONObject data) {
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
        } catch (JSONException ex) { return null; }
    }

    public String getType() {
        return type;
    }
}