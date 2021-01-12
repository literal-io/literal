(function highlightSelectors() {
  const highlightClassName = "literal-highlight";
  const selectors = JSON.parse('${PARAM_SELECTORS}');

  // remove any existing selectors
  document.querySelectorAll(`.${highlightClassName}`).forEach((elem) => {
    const parentNode = elem.parentNode;
    elem.replaceWith(...Array.from(elem.childNodes));
    parentNode.normalize();
  });

  selectors.forEach(({ startSelector, endSelector }) => {
    const startNode = document.evaluate(
      startSelector.value,
      document,
      null,
      XPathResult.FIRST_ORDERED_NODE_TYPE,
      null
    ).singleNodeValue;
    const endNode = document.evaluate(
      endSelector.value,
      document,
      null,
      XPathResult.FIRST_ORDERED_NODE_TYPE,
      null
    ).singleNodeValue;

    const range = document.createRange();
    range.setStart(startNode, startSelector.refinedBy[0].start);
    range.setEnd(endNode, endSelector.refinedBy[0].end);

    const highlightSpan = document.createElement("span");
    highlightSpan.classList.add(highlightClassName);
    highlightSpan.style.backgroundColor = "#000";
    highlightSpan.style.color = "rgba(255, 255, 255, 0.92)";

    try {
      range.surroundContents(highlightSpan);
    } catch (e) {
      // tried to partially select a non-text node
      // range.extractContents and reinsert where it belongs?
      // or manually wrap end and start nodes when non text, and advance
      // selection to only wrap full nodes
    }
  });
})();
