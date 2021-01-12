(function getSelector() {
  // https://github.com/chromium/chromium/blob/77578ccb4082ae20a9326d9e673225f1189ebb63/third_party/blink/renderer/devtools/front_end/elements/DOMPath.js#L242
  const getXPath = (function() {
    const Elements = { DOMPath: {} };
    Elements.DOMPath.xPath = function(node, optimized) {
      if (node.nodeType === Node.DOCUMENT_NODE) return "/";

      const steps = [];
      let contextNode = node;
      while (contextNode) {
        const step = Elements.DOMPath._xPathValue(contextNode, optimized);
        if (!step) break; // Error - bail out early.
        steps.push(step);
        if (step.optimized) break;
        contextNode = contextNode.parentNode;
      }

      steps.reverse();
      return (steps.length && steps[0].optimized ? "" : "/") + steps.join("/");
    };

    Elements.DOMPath._xPathValue = function(node, optimized) {
      let ownValue;
      const ownIndex = Elements.DOMPath._xPathIndex(node);
      if (ownIndex === -1) return null; // Error.

      switch (node.nodeType) {
        case Node.ELEMENT_NODE:
          if (optimized && node.getAttribute("id"))
            return new Elements.DOMPath.Step(
              '//*[@id="' + node.getAttribute("id") + '"]',
              true
            );
          ownValue = node.localName;
          break;
        case Node.ATTRIBUTE_NODE:
          ownValue = "@" + node.nodeName;
          break;
        case Node.TEXT_NODE:
        case Node.CDATA_SECTION_NODE:
          ownValue = "text()";
          break;
        case Node.PROCESSING_INSTRUCTION_NODE:
          ownValue = "processing-instruction()";
          break;
        case Node.COMMENT_NODE:
          ownValue = "comment()";
          break;
        case Node.DOCUMENT_NODE:
          ownValue = "";
          break;
        default:
          ownValue = "";
          break;
      }

      if (ownIndex > 0) ownValue += "[" + ownIndex + "]";

      return new Elements.DOMPath.Step(
        ownValue,
        node.nodeType === Node.DOCUMENT_NODE
      );
    };

    Elements.DOMPath._xPathIndex = function(node) {
      // Returns -1 in case of error, 0 if no siblings matching the same expression, <XPath index among the same expression-matching sibling nodes> otherwise.
      function areNodesSimilar(left, right) {
        if (left === right) return true;

        if (
          left.nodeType === Node.ELEMENT_NODE &&
          right.nodeType === Node.ELEMENT_NODE
        )
          return left.localName === right.localName;

        if (left.nodeType === right.nodeType) return true;

        // XPath treats CDATA as text nodes.
        const leftType =
          left.nodeType === Node.CDATA_SECTION_NODE
            ? Node.TEXT_NODE
            : left.nodeType;
        const rightType =
          right.nodeType === Node.CDATA_SECTION_NODE
            ? Node.TEXT_NODE
            : right.nodeType;
        return leftType === rightType;
      }

      const siblings = node.parentNode ? node.parentNode.childNodes : null;
      if (!siblings) return 0; // Root node - no siblings.
      let hasSameNamedElements;
      for (let i = 0; i < siblings.length; ++i) {
        if (areNodesSimilar(node, siblings[i]) && siblings[i] !== node) {
          hasSameNamedElements = true;
          break;
        }
      }
      if (!hasSameNamedElements) return 0;
      let ownIndex = 1; // XPath indices start with 1.
      for (let i = 0; i < siblings.length; ++i) {
        if (areNodesSimilar(node, siblings[i])) {
          if (siblings[i] === node) return ownIndex;
          ++ownIndex;
        }
      }
      return -1; // An error occurred: |node| not found in parent's children.
    };

    Elements.DOMPath.Step = class {
      constructor(value, optimized) {
        this.value = value;
        this.optimized = optimized || false;
      }

      toString() {
        return this.value;
      }
    };

    return Elements.DOMPath.xPath;
  })();

  const range = window.getSelection().getRangeAt(0);
  const selectorFromRangeBoundary = ({
    container,
    startPosition,
    endPosition,
  }) => ({
    type: "XPATH_SELECTOR",
    value: getXPath(container),
    refinedBy: [
      {
        type: "TEXT_POSITION_SELECTOR",
        start: startPosition,
        end: endPosition,
      },
    ],
  });

  const output = {
    type: "RANGE_SELECTOR",
    startSelector: selectorFromRangeBoundary({
      container: range.startContainer,
      startPosition: range.startOffset,
      endPosition: range.startContainer.textContent.length,
    }),
    endSelector: selectorFromRangeBoundary({
      container: range.endContainer,
      startPosition: 0,
      endPosition: range.endOffset,
    }),
  };

  window.getSelection().removeAllRanges() 

  return output;
})();
