package io.literal.model;

import com.google.gson.JsonObject;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Collections;

import io.literal.lib.AnnotationCollectionLib;
import io.literal.lib.Constants;
import io.literal.lib.JsonArrayUtil;
import type.AnnotationBodyInput;
import type.Motivation;
import type.TextualBodyInput;
import type.TextualBodyType;

public class TextualBody extends Body {
    private final String id;
    private final Format format;
    private final Language language;
    private final Language processingLanguage;
    private final TextDirection textDirection;
    private final String[] accessibility;
    private final String[] rights;
    private final String value;

    public String getValue() {
        return value;
    }

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
        output.put("format", this.format != null ? this.format.name() : null);
        output.put("language", this.language != null ? this.language.name() : null);
        output.put("processingLanguage", this.processingLanguage != null ? this.processingLanguage.name() : null);
        output.put("textDirection", this.textDirection != null ? this.textDirection.name() : null);
        output.put("accessibility", this.accessibility != null ? new JSONArray(this.accessibility) : null);
        output.put("rights", this.rights != null ? new JSONArray(this.rights) : null);
        output.put("value", this.value);
        output.put("type", this.type.name());

        return output;
    }

    @Override
    public AnnotationBodyInput toAnnotationBodyInput() {
        return AnnotationBodyInput.builder()
                .textualBody(
                        TextualBodyInput
                                .builder()
                                .id(this.id)
                                .value(this.value)
                                .format(this.format.toGraphQL())
                                .textDirection(this.textDirection.toGraphQL())
                                .language(this.language.toGraphQL())
                                .purpose(Collections.singletonList(Motivation.TAGGING))
                                .type(TextualBodyType.TEXTUAL_BODY)
                                .build()
                )
                .build();
    }

    public static TextualBody fromJson(JSONObject json) throws JSONException {
        return new TextualBody(
                json.optString("id"),
                !json.isNull("format") ? Format.valueOf(json.getString("format")) : null,
                !json.isNull("language") ? Language.valueOf(json.getString("language")) : null,
                !json.isNull("processingLanguage") ? Language.valueOf(json.getString("processingLanguage")) : null,
                !json.isNull("textDirection") ? TextDirection.valueOf(json.getString("textDirection")) : null,
                !json.isNull("accessibility") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("accessibility")) : null,
                !json.isNull("rights") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("rights")) : null,
                json.getString("value")
        );
    }
}
