package io.literal.model;

import android.util.Log;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;

import io.literal.lib.Crypto;
import io.literal.lib.JsonArrayUtil;
import type.AnnotationAddOperationInput;
import type.AnnotationSetOperationInput;
import type.AnnotationTargetInput;
import type.AnnotationWhereInput;
import type.ExternalTargetInput;
import type.PatchAnnotationOperationInput;

public class ExternalTarget extends Target {

    private final String id;
    private final Format format;
    private final Language language;
    private final Language processingLanguage;
    private final TextDirection textDirection;
    private final ResourceType resourceType;
    private final String[] accessibility;
    private final String[] rights;

    public ExternalTarget(@NotNull String id, Format format, Language language, Language processingLanguage, TextDirection textDirection, String[] accessibility, String[] rights, ResourceType resourceType) {
        super(Type.EXTERNAL_TARGET);
        this.id = id;
        this.format = format;
        this.language = language;
        this.processingLanguage = processingLanguage;
        this.textDirection = textDirection;
        this.accessibility = accessibility;
        this.rights = rights;
        this.resourceType = resourceType;
    }

    public String getId() {
        return id;
    }
    public Format getFormat() { return format; }
    public Language getLanguage() { return language; }
    public Language getProcessingLanguage() { return processingLanguage; }
    public ResourceType getResourceType() { return resourceType; }
    public String[] getAccessibility() { return accessibility; }
    public String[] getRights() { return rights; }
    public TextDirection getTextDirection() { return textDirection; }

    public static ExternalTarget fromJson(JSONObject json) throws JSONException {
        return new ExternalTarget(
                json.optString("id"),
                !json.isNull("format") ? Format.valueOf(json.getString("format")) : null,
                !json.isNull("language") ? Language.valueOf(json.getString("language")) : null,
                !json.isNull("processingLanguage") ? Language.valueOf(json.getString("processingLanguage")) : null,
                !json.isNull("textDirection") ? TextDirection.valueOf(json.getString("textDirection")) : null,
                !json.isNull("accessibility") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("accessibility")) : null,
                !json.isNull("rights") ? JsonArrayUtil.parseJsonStringArray(json.getJSONArray("rights")) : null,
                !json.isNull("type") ? ResourceType.valueOf(json.getString("type")) : null
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
        output.put("type", this.resourceType);

        return output;
    }

    @Override
    public AnnotationTargetInput toAnnotationTargetInput() {
        String hashId = null;
        try {
            hashId = Crypto.sha256Hex(this.id);
        } catch (Exception e) {
            Log.d("ExternalTarget", "Unable to hash id", e);
        }

        return AnnotationTargetInput.builder().externalTarget(
                ExternalTargetInput.builder()
                        .id(this.id)
                        .format(this.format != null ? this.format.toGraphQL() : null)
                        .language(this.language != null ? this.language.toGraphQL() : null)
                        .processingLanguage(this.processingLanguage != null ? this.processingLanguage.toGraphQL() : null)
                        .textDirection(this.textDirection != null ? this.textDirection.toGraphQL() : null)
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
                        .hashId(hashId)
                        .type(
                                this.resourceType != null ? this.resourceType.toGraphQL() : null
                        )
                        .build()
        ).build();
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
