package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import io.literal.lib.JsonArrayUtil;

public class ExternalTarget extends Target {

    private final String id;
    private final Format format;
    private final Language language;
    private final Language processingLanguage;
    private final TextDirection textDirection;
    private final String[] accessibility;
    private final String[] rights;

    public ExternalTarget(@NotNull String id, Format format, Language language, Language processingLanguage, TextDirection textDirection, String[] accessibility, String[] rights) {
        super(Type.EXTERNAL_TARGET);
        this.id = id;
        this.format = format;
        this.language = language;
        this.processingLanguage = processingLanguage;
        this.textDirection = textDirection;
        this.accessibility = accessibility;
        this.rights = rights;
    }

    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("format", this.format.name());
        output.put("language", this.language.name());
        output.put("procesisngLanguage", this.processingLanguage.name());
        output.put("textDirection", this.textDirection.name());
        output.put("accessibility", this.accessibility != null ? new JSONArray(this.accessibility) : null);
        output.put("rights", this.rights != null ? new JSONArray(this.rights) : null);

        return output;
    }

    public static ExternalTarget fromJson(JSONObject json) throws JSONException {
        return new ExternalTarget(
                json.optString("id"),
                json.has("format") ? Format.valueOf(json.getString("format")) : null,
                json.has("language") ? Language.valueOf(json.getString("language")) : null,
                json.has("processingLanguage") ? Language.valueOf(json.getString("processingLanguage")) : null,
                json.has("textDirection") ? TextDirection.valueOf(json.getString("textDirection")) : null,
                json.has("accessibility") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("accessibility")) : null,
                json.has("rights") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("rights")) : null
        );
    }
}
