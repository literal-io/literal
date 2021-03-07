export const evaluate = (value) =>
  document.evaluate(
    value,
    document,
    null,
    XPathResult.FIRST_ORDERED_NODE_TYPE,
    null
  ).singleNodeValue;

export const xPathRangeSelectorPredicate = ({
  type,
  startSelector,
  endSelector,
}) =>
  type === "RANGE_SELECTOR" &&
  startSelector.type === "XPATH_SELECTOR" &&
  (startSelector.refinedBy || []).some(
    (refinedBySelector) => refinedBySelector.type === "TEXT_POSITION_SELECTOR"
  ) &&
  endSelector.type === "XPATH_SELECTOR" &&
  (endSelector.refinedBy || []).some(
    (refinedBySelector) => refinedBySelector.type === "TEXT_POSITION_SELECTOR"
  );
