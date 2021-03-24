package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;

import io.literal.lib.JsonArrayUtil;
import type.StateInput;
import type.TimeStateInput;
import type.TimeStateType;

public class TimeState extends State {

    private final String[] cached;
    private final String[] sourceDate;

    public TimeState(@NotNull String[] cached, @NotNull String[] sourceDate) {
        super(Type.TIME_STATE);
        this.cached = cached;
        this.sourceDate = sourceDate;
    }

    public String[] getCached() {
        return cached;
    }

    public String[] getSourceDate() {
        return sourceDate;
    }

    public static TimeState fromJson(JSONObject json) throws JSONException {
        return new TimeState(
                JsonArrayUtil.parseJsonStringArray(json.getJSONArray("cached")),
                JsonArrayUtil.parseJsonStringArray(json.getJSONArray("sourceDate"))
        );
    }

    public JSONObject toJson() throws JSONException {
        JSONObject result = new JSONObject();

        result.put("type", this.getType().name());
        result.put("cached", this.cached);
        result.put("sourceDate", this.sourceDate);

        return result;
    }

    public StateInput toStateInput() {
        return StateInput.builder()
                .timeState(
                        TimeStateInput.builder()
                                .type(TimeStateType.TIME_STATE)
                                .cached(Arrays.asList(this.cached))
                                .sourceDate(Arrays.asList(this.sourceDate))
                                .build()
                )
                .build();
    }
}
