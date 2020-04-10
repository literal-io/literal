package io.literal.lib;

import org.json.JSONObject;

public class WebEvent {

    public static final String TYPE_ACTIVITY_FINISH = "ACTIVITY_FINISH";

    private String type;
    private String pid;

    public WebEvent (JSONObject data) {
        this.type = data.optString("type");
        this.pid = data.optString("pid");
    }

    public String getType() {
        return type;
    }
}