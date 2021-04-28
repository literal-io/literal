package io.literal.model;

import android.system.ErrnoException;
import android.util.Log;

import androidx.annotation.Nullable;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.security.NoSuchAlgorithmException;
import java.util.Arrays;
import java.util.stream.Collectors;

import io.literal.lib.AnnotationCollectionLib;
import io.literal.lib.Constants;
import io.literal.lib.Crypto;
import io.literal.lib.JsonArrayUtil;
import io.literal.repository.ErrorRepository;
import type.AnnotationAddOperationInput;
import type.AnnotationBodyInput;
import type.AnnotationRemoveOperationInput;
import type.AnnotationWhereInput;
import type.PatchAnnotationOperationInput;
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
    private final Motivation[] purpose;
    private final String value;

    public TextualBody(String id, Format format, Language language, Language processingLanguage, TextDirection textDirection, String[] accessibility, String[] rights, Motivation[] purpose, @NotNull String value) {
        super(Type.TEXTUAL_BODY);

        String idWithDefault = id;
        if (id == null || id.equals("")) {
            try {
                idWithDefault = Crypto.sha256Hex(value);
            } catch (NoSuchAlgorithmException ex) {
                ErrorRepository.captureException(ex);
            }
        }

        this.id = idWithDefault;
        this.format = format;
        this.language = language;
        this.processingLanguage = processingLanguage;
        this.textDirection = textDirection;
        this.accessibility = accessibility;
        this.purpose = purpose;
        this.rights = rights;
        this.value = value;
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
                !json.isNull("purpose")
                        ? Arrays.stream(JsonArrayUtil.parseJsonStringArray(json.getJSONArray("purpose")))
                        .map(Motivation::valueOf).toArray(Motivation[]::new)
                        : null,
                json.getString("value")
        );
    }

    public static TextualBody createTag(String text, String creatorUsername) {
        String annotationCollectionId = AnnotationCollectionLib.makeId(
                creatorUsername,
                text
        );
        return new TextualBody(
            annotationCollectionId,
                Format.TEXT_PLAIN,
                Language.EN_US,
                Language.EN_US,
                TextDirection.LTR,
                null,
                null,
                new Motivation[] { Motivation.TAGGING },
                text
        );
    }

    public String getValue() {
        return value;
    }

    public String getId() {
        return id;
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
        output.put("purpose", this.purpose != null ? new JSONArray(Arrays.stream(this.purpose).map(Motivation::name).collect(Collectors.toList())) : null);
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
                                .purpose(
                                        this.purpose != null
                                                ? Arrays.stream(this.purpose).map(Motivation::toGraphQL).collect(Collectors.toList())
                                                : null
                                )
                                .type(TextualBodyType.TEXTUAL_BODY)
                                .build()
                )
                .build();
    }

    public PatchAnnotationOperationInput toPatchAnnotationOperationInputRemove() {
        return PatchAnnotationOperationInput.builder()
                .remove(
                        AnnotationRemoveOperationInput.builder()
                                .body(true)
                                .where(
                                        AnnotationWhereInput.builder()
                                                .id(id)
                                                .build()
                                )
                                .build()
                )
                .build();
    }

    public PatchAnnotationOperationInput toPatchAnnotationOperationInputAdd() {
        return PatchAnnotationOperationInput.builder()
                .add(
                        AnnotationAddOperationInput.builder()
                                .body(
                                        this.toAnnotationBodyInput()
                                )
                                .build()
                )
                .build();
    }

    @Override
    public boolean equals(@Nullable Object obj) {
        if (obj.getClass() != this.getClass()) {
            return false;
        }

        return ((TextualBody) obj).getId().equals(id);
    }
}
