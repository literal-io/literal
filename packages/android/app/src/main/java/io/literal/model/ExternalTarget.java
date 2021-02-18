package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;

import io.literal.lib.JsonArrayUtil;
import type.AnnotationTargetInput;
import type.ExternalTargetInput;

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

    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("format", this.format != null ? this.format.name() : null);
        output.put("language", this.language != null ? this.language.name() : null);
        output.put("processingLanguage", this.processingLanguage != null ? this.processingLanguage.name() : null);
        output.put("textDirection", this.textDirection != null ? this.textDirection.name() : null);
        output.put("accessibility", this.accessibility != null ? new JSONArray(this.accessibility) : null);
        output.put("rights", this.rights != null ? new JSONArray(this.rights) : null);
        output.put("type", this.type.name());

        return output;
    }

    @Override
    public AnnotationTargetInput toAnnotationTargetInput() {
        return AnnotationTargetInput.builder().externalTarget(
                ExternalTargetInput.builder()
                        .id(this.id)
                        .format(this.format.toGraphQL())
                        .language(this.language.toGraphQL())
                        .processingLanguage(this.processingLanguage.toGraphQL())
                        .textDirection(this.textDirection.toGraphQL())
                        .accessibility(
                                this.accessibility != null
                                        ? Arrays.asList(this.accessibility)
                                        : null
                        )
                        .rights(
                                this.rights != null
                                        ? Arrays.asList(this.rights)
                                        : null
                        )
                        .build()
        ).build();
    }
}
