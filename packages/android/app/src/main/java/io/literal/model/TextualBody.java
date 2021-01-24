package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import io.literal.lib.JsonArrayUtil;

public class TextualBody extends Body {
    private final String id;
    private final Format format;
    private final Language language;
    private final Language processingLanguage;
    private final TextDirection textDirection;
    private final String[] accessibility;
    private final String[] rights;
    private final String value;

    public TextualBody(String id, Format format, Language language, Language processingLanguage, TextDirection textDirection, String[] accessibility, String[] rights, @NotNull String value) {
        super(Type.TEXTUAL_BODY);
        this.id = id;
        this.format = format;
        this.language = language;
        this.processingLanguage = processingLanguage;
        this.textDirection = textDirection;
        this.accessibility = accessibility;
        this.rights = rights;
        this.value = value;
    }

    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("format", this.format.name());
        output.put("language", this.language.name());
        output.put("procesisngLanguage", this.processingLanguage.name());
        output.put("textDirection", this.textDirection.name());
        output.put("accessibility", new JSONArray(this.accessibility));
        output.put("rights", new JSONArray(this.rights));
        output.put("value", this.value);

        return output;
    }

    public static TextualBody fromJson(JSONObject json) throws JSONException {
        return new TextualBody(
                json.optString("id"),
                json.has("format") ? Format.valueOf(json.getString("format")) : null,
                json.has("language") ? Language.valueOf(json.getString("language")) : null,
                json.has("processingLanguage") ? Language.valueOf(json.getString("processingLanguage")) : null,
                json.has("textDirection") ? TextDirection.valueOf(json.getString("textDirection")) : null,
                json.has("accessibility") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("accessibility")) : null,
                json.has("rights") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("rights")) : null,
                json.getString("value")
        );
    }
}
