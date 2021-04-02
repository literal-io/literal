package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;

import io.literal.lib.JsonArrayUtil;
import type.AnnotationAddOperationInput;
import type.AnnotationSetOperationInput;
import type.AnnotationTargetInput;
import type.AnnotationWhereInput;
import type.PatchAnnotationOperationInput;
import type.TextualTargetInput;

public class TextualTarget extends Target {

    private final String id;
    private final Format format;
    private final Language language;
    private final Language processingLanguage;
    private final TextDirection textDirection;
    private final String[] accessibility;
    private final String[] rights;
    private final String value;

    public TextualTarget(String id, Format format, Language language, Language processingLanguage, TextDirection textDirection, String[] accessibility, String[] rights, @NotNull String value) {
        super(Type.TEXTUAL_TARGET);
        this.id = id == null || id.equals("") ? UUID.randomUUID().toString() : id;
        this.format = format;
        this.language = language;
        this.processingLanguage = processingLanguage;
        this.textDirection = textDirection;
        this.accessibility = accessibility;
        this.rights = rights;
        this.value = value;
    }

    public static TextualTarget fromJson(JSONObject json) throws JSONException {
        return new TextualTarget(
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

    public String getValue() {
        return value;
    }
    public String getId() { return id; }
    public TextDirection getTextDirection() { return textDirection; }
    public String[] getRights() { return rights; }
    public String[] getAccessibility() { return accessibility; }
    public Language getProcessingLanguage() { return processingLanguage; }
    public Format getFormat() { return format; }
    public Language getLanguage() { return language; }

    @Override
    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("format", this.format.name());
        output.put("language", this.language.name());
        output.put("processingLanguage", this.processingLanguage.name());
        output.put("textDirection", this.textDirection.name());
        output.put("accessibility", this.accessibility != null ? new JSONArray(this.accessibility) : null);
        output.put("rights", this.rights != null ? new JSONArray(this.rights) : null);
        output.put("value", this.value);
        output.put("type", this.type.name());

        return output;
    }

    @Override
    public AnnotationTargetInput toAnnotationTargetInput() {
        return AnnotationTargetInput
                .builder()
                .textualTarget(
                        TextualTargetInput
                                .builder()
                                .format(this.format.toGraphQL())
                                .language(this.language.toGraphQL())
                                .processingLanguage(this.language.toGraphQL())
                                .textDirection(this.textDirection.toGraphQL())
                                .value(this.value)
                                .id(this.id)
                                .build()
                )
                .build();
    }

    @Override
    public PatchAnnotationOperationInput toPatchAnnotationOperationInputAdd() {
        return PatchAnnotationOperationInput.builder()
                .add(
                        AnnotationAddOperationInput
                                .builder()
                                .target(
                                        this.toAnnotationTargetInput()
                                )
                                .build()
                )
                .build();
    }

    @Override
    public PatchAnnotationOperationInput toPatchAnnotationOperationInputSet() {
        return PatchAnnotationOperationInput.builder()
                .set(
                        AnnotationSetOperationInput.builder()
                                .where(
                                        AnnotationWhereInput.builder()
                                                .id(this.id)
                                                .build()
                                )
                                .target(this.toAnnotationTargetInput())
                                .build()
                )
                .build();
    }
}
