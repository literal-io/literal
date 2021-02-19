package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.Collections;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import io.literal.lib.AnnotationLib;
import io.literal.lib.JsonArrayUtil;
import type.AnnotationType;
import type.CreateAnnotationInput;

public class Annotation {
    private final Body[] body;
    private final Target[] target;
    private final Motivation[] motivation;
    private final String id;

    public Annotation(Body[] body, @NotNull Target[] target, Motivation[] motivation, String id) {
        this.body = body;
        this.target = target;
        this.motivation = motivation;
        this.id = id;
    }

    public static Annotation fromJson(JSONObject json) throws JSONException {
        return new Annotation(
                !json.isNull("body")
                        ? JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("body"), new Body[0], Body::fromJson)
                        : null,
                JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("target"), new Target[0], Target::fromJson),
                !json.isNull("motivation")
                        ? Arrays.stream(JsonArrayUtil.parseJsonStringArray(json.getJSONArray("motivation")))
                        .map(Motivation::valueOf).toArray(Motivation[]::new)
                        : null,
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

    public Motivation[] getMotivation() { return motivation; }

    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("body", this.body != null ? JsonArrayUtil.stringifyObjectArray(this.body, Body::toJson) : null);
        output.put("target", JsonArrayUtil.stringifyObjectArray(this.target, Target::toJson));
        output.put("motivation", this.motivation != null ? new JSONArray(Arrays.stream(this.motivation).map(Motivation::name).collect(Collectors.toList())) : null);

        return output;
    }

    public CreateAnnotationInput toCreateAnnotationInput() {
        return CreateAnnotationInput.builder()
                .context(Collections.singletonList("http://www.w3.org/ns/anno.jsonld"))
                .type(Collections.singletonList(AnnotationType.ANNOTATION))
                .motivation(
                        this.motivation != null
                                ? Arrays.stream(this.motivation).map(Motivation::toGraphQL).collect(Collectors.toList())
                                : null
                )
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
