package io.literal.model;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.stream.Collectors;
import java.util.stream.Stream;

import io.literal.lib.JsonArrayUtil;
import type.SelectorInput;
import type.XPathSelectorInput;
import type.XPathSelectorType;

public class XPathSelector extends Selector {

    private final String value;
    private final Selector[] refinedBy;

    public XPathSelector(@NotNull String value) {
        super(Type.XPATH_SELECTOR);
        this.value = value;
        this.refinedBy = null;
    }

    public XPathSelector(@NotNull String value, Selector[] refinedBy) {
        super(Type.XPATH_SELECTOR);
        this.value = value;
        this.refinedBy = refinedBy;
    }

    public String getValue() {
        return value;
    }

    public Selector[] getRefinedBy() {
        return refinedBy;
    }

    public static XPathSelector fromJson(JSONObject json) throws JSONException {
        return new XPathSelector(
                json.getString("value"),
                !json.isNull("refinedBy")
                    ? JsonArrayUtil.parseJsonObjectArray(json.getJSONArray("refinedBy"), new Selector[0], Selector::fromJson) : null

        );
    }

    @Override
    public JSONObject toJson() throws JSONException {
        JSONObject result = new JSONObject();

        result.put("type", getType());
        result.put("value", getValue());
        result.put("refinedBy", this.refinedBy != null ? JsonArrayUtil.stringifyObjectArray(this.refinedBy, Selector::toJson) : null);

        return result;
    }

    @Override
    public SelectorInput toSelectorInput() {
        return SelectorInput.builder()
                .xPathSelector(
                        XPathSelectorInput.builder()
                                .type(XPathSelectorType.XPATH_SELECTOR)
                                .value(this.value)
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
