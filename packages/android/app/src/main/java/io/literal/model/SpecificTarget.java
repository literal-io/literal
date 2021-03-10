package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import io.literal.lib.JsonArrayUtil;
import type.AnnotationAddOperationInput;
import type.AnnotationSetOperationInput;
import type.AnnotationTargetInput;
import type.AnnotationWhereInput;
import type.PatchAnnotationOperationInput;
import type.SpecificResourceType;
import type.SpecificTargetInput;

public class SpecificTarget extends Target {

    private final String id;
    private final Target source;
    private final Selector[] selector;

    public SpecificTarget(String id, @NotNull Target source, @NotNull Selector[] selector) {
        super(Type.SPECIFIC_TARGET);

        this.id = id == null || id.equals("") ? UUID.randomUUID().toString() : id;
        this.source = source;
        this.selector = selector;
    }

    public static SpecificTarget fromJson(JSONObject json) throws JSONException {
        return new SpecificTarget(
                json.optString("id"),
                Target.fromJson(json.getJSONObject("source")),
                JsonArrayUtil.parseJsonObjectArray(
                        json.getJSONArray("selector"),
                        new Selector[0],
                        Selector::fromJson
                )
        );
    };

    public String getId() {
        return id;
    }

    public Target getSource() {
        return source;
    }

    public Selector[] getSelector() {
        return selector;
    }

    @Override
    public JSONObject toJson() throws JSONException {
        JSONObject output = new JSONObject();
        output.put("id", this.id);
        output.put("source", this.source.toJson());
        output.put("selector", JsonArrayUtil.stringifyObjectArray(
                this.selector,
                Selector::toJson
        ));
        output.put("type", SpecificResourceType.SPECIFIC_RESOURCE.name());

        return output;
    }

    @Override
    public AnnotationTargetInput toAnnotationTargetInput() {
        return AnnotationTargetInput.builder().specificTarget(
                SpecificTargetInput.builder()
                        .id(this.id)
                        .source(this.source.toAnnotationTargetInput())
                        .selector(
                                Stream.of(this.selector).map(Selector::toSelectorInput).collect(Collectors.toList())
                        )
                        .type(SpecificResourceType.SPECIFIC_RESOURCE)
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
