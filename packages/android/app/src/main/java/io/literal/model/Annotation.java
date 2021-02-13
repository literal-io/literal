package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Collections;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import io.literal.lib.AnnotationLib;
import io.literal.lib.JsonArrayUtil;
import type.AnnotationType;
import type.CreateAnnotationInput;
import type.Motivation;

public class Annotation {
    private final Body[] body;
    private final Target[] target;
    private final String id;

    public Annotation(Body[] body, @NotNull Target[] target, String id) {
        this.body = body;
        this.target = target;
        this.id = id;
    }

    public static Annotation fromJson(JSONObject json) throws JSONException {
        return new Annotation(
                json.has("body")
                        ? JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("body"), new Body[0], Body::fromJson)
                        : null,
                JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("target"), new Target[0], Target::fromJson),
                json.optString("id", null)
        );
    }

    public String getId() {
        return id;
    }

    public Target[] getTarget() {
        return target;
    }

    public Body[] getBody() {
        return body;
    }

    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("body", this.body != null ? JsonArrayUtil.stringifyObjectArray(this.body, Body::toJson) : null);
        output.put("target", JsonArrayUtil.stringifyObjectArray(this.target, Target::toJson));
        return output;
    }

    public CreateAnnotationInput toCreateAnnotationInput() {
        return CreateAnnotationInput.builder()
                .context(Collections.singletonList("http://www.w3.org/ns/anno.jsonld"))
                .type(Collections.singletonList(AnnotationType.ANNOTATION))
                .motivation(Collections.singletonList(Motivation.HIGHLIGHTING))
                .id(this.id)
                .creatorUsername(AnnotationLib.creatorUsernameFromId(this.id))
                .target(
                        Stream.of(this.target).map(Target::toAnnotationTargetInput).collect(Collectors.toList())
                )
                .body(
                        this.body != null
                                ? Stream.of(this.body).map(Body::toAnnotationBodyInput).collect(Collectors.toList())
                                : null
                ).build();
    }
}
