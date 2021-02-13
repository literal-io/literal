package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.stream.Collectors;
import java.util.stream.Stream;

import io.literal.lib.JsonArrayUtil;
import type.RangeSelectorInput;
import type.RangeSelectorType;
import type.SelectorInput;

public class RangeSelector extends Selector {

    private final Selector startSelector;
    private final Selector endSelector;
    private final Selector[] refinedBy;

    public RangeSelector(@NotNull Selector startSelector, @NotNull Selector endSelector, Selector[] refinedBy) {
        super(Selector.Type.RANGE_SELECTOR);
        this.startSelector = startSelector;
        this.endSelector = endSelector;
        this.refinedBy = refinedBy;
    }

    public RangeSelector(@NotNull Selector startSelector, @NotNull Selector endSelector) {
        super(Type.RANGE_SELECTOR);
        this.startSelector = startSelector;
        this.endSelector = endSelector;
        this.refinedBy = null;
    }

    public static RangeSelector fromJson(JSONObject json) throws JSONException {
        return new RangeSelector(
                Selector.fromJson(json.getJSONObject("startSelector")),
                Selector.fromJson(json.getJSONObject("endSelector")),
                json.has("refinedBy")
                        ? JsonArrayUtil.parseJsonObjectArray(
                        json.getJSONArray("refinedBy"),
                        new Selector[0],
                        Selector::fromJson
                )
                        : null
        );
    }

    public Selector getStartSelector() {
        return startSelector;
    }

    public Selector getEndSelector() {
        return endSelector;
    }

    public Selector[] getRefinedBy() {
        return refinedBy;
    }

    public JSONObject toJson() throws JSONException {
        JSONObject result = new JSONObject();

        result.put("type", this.getType().toString());
        result.put("startSelector", this.getStartSelector().toJson());
        result.put("endSelector", this.getEndSelector().toJson());
        result.put("refinedBy",
                this.refinedBy != null ? JsonArrayUtil.stringifyObjectArray(this.refinedBy, Selector::toJson) : null);

        return result;
    }

    @Override
    public SelectorInput toSelectorInput() {
        return SelectorInput.builder()
                .rangeSelector(
                        RangeSelectorInput.builder()
                                .type(RangeSelectorType.RANGE_SELECTOR)
                                .startSelector(this.startSelector.toSelectorInput())
                                .endSelector(this.endSelector.toSelectorInput())
                                .refinedBy(
                                        this.refinedBy != null
                                                ? Stream.of(this.refinedBy).map(Selector::toSelectorInput).collect(Collectors.toList())
                                                : null
                                )
                                .build()
                )
                .build();
    }
}
