package io.literal.model;

import org.jetbrains.annotations.NotNull;

public class RangeSelector<RangeType extends Selector, RefinedByType extends Selector> extends Selector {

    private final RangeType startSelector;
    private final RangeType endSelector;
    private final RefinedByType[] refinedBy;

    public RangeSelector(@NotNull RangeType startSelector,  @NotNull RangeType endSelector, RefinedByType[] refinedBy) {
        super(Selector.Type.RANGE_SELECTOR);
        this.startSelector = startSelector;
        this.endSelector  = endSelector;
        this.refinedBy = refinedBy;
    }

    public RangeSelector(@NotNull RangeType startSelector, @NotNull RangeType endSelector) {
        super(Type.RANGE_SELECTOR);
        this.startSelector = startSelector;
        this.endSelector = endSelector;
        this.refinedBy = null;
    }

    public RangeType getStartSelector() {
        return startSelector;
    }

    public RangeType getEndSelector() {
        return endSelector;
    }

    public RefinedByType[] getRefinedBy() {
        return refinedBy;
    }
}
