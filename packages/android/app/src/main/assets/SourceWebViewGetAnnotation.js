var Literal;
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	// The require scope
/******/ 	var __webpack_require__ = {};
/******/ 	
/************************************************************************/
/******/ 	/* webpack/runtime/define property getters */
/******/ 	(() => {
/******/ 		// define getter functions for harmony exports
/******/ 		__webpack_require__.d = (exports, definition) => {
/******/ 			for(var key in definition) {
/******/ 				if(__webpack_require__.o(definition, key) && !__webpack_require__.o(exports, key)) {
/******/ 					Object.defineProperty(exports, key, { enumerable: true, get: definition[key] });
/******/ 				}
/******/ 			}
/******/ 		};
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/hasOwnProperty shorthand */
/******/ 	(() => {
/******/ 		__webpack_require__.o = (obj, prop) => (Object.prototype.hasOwnProperty.call(obj, prop))
/******/ 	})();
/******/ 	
/******/ 	/* webpack/runtime/make namespace object */
/******/ 	(() => {
/******/ 		// define __esModule on exports
/******/ 		__webpack_require__.r = (exports) => {
/******/ 			if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 				Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 			}
/******/ 			Object.defineProperty(exports, '__esModule', { value: true });
/******/ 		};
/******/ 	})();
/******/ 	
/************************************************************************/
var __webpack_exports__ = {};
// ESM COMPAT FLAG
__webpack_require__.r(__webpack_exports__);

// EXPORTS
__webpack_require__.d(__webpack_exports__, {
  "default": () => (/* binding */ get_annotation)
});

;// CONCATENATED MODULE: ./get-annotation/xpath.mjs
// https://github.com/chromium/chromium/blob/77578ccb4082ae20a9326d9e673225f1189ebb63/third_party/blink/renderer/devtools/front_end/elements/DOMPath.js#L242

const xPath = function(node, optimized) {
  if (node.nodeType === Node.DOCUMENT_NODE) return "/";

  const steps = [];
  let contextNode = node;
  while (contextNode) {
    const step = _xPathValue(contextNode, optimized);
    if (!step) break; // Error - bail out early.
    steps.push(step);
    if (step.optimized) break;
    contextNode = contextNode.parentNode;
  }

  steps.reverse();
  return (steps.length && steps[0].optimized ? "" : "/") + steps.join("/");
};

const _xPathValue = function(node, optimized) {
  let ownValue;
  const ownIndex = _xPathIndex(node);
  if (ownIndex === -1) return null; // Error.

  switch (node.nodeType) {
    case Node.ELEMENT_NODE:
      if (optimized && node.getAttribute("id"))
        return new Step('//*[@id="' + node.getAttribute("id") + '"]', true);
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

  return new Step(ownValue, node.nodeType === Node.DOCUMENT_NODE);
};

const _xPathIndex = function(node) {
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

const Step = class {
  constructor(value, optimized) {
    this.value = value;
    this.optimized = optimized || false;
  }

  toString() {
    return this.value;
  }
};

;// CONCATENATED MODULE: ./get-annotation/model.mjs


const LANGUAGE = "EN_US";
const TEXT_DIRECTION = getComputedStyle(
  window.document.body
).direction.toUpperCase();

const makeXPathSelectorFromRange = ({
  container,
  startPosition,
  endPosition,
}) => ({
  type: "XPATH_SELECTOR",
  value: xPath(container),
  refinedBy: [
    {
      type: "TEXT_POSITION_SELECTOR",
      start: startPosition,
      end: endPosition,
    },
  ],
});

const makeAnnotationFromSelection = ({ selection }) => {
  const range = selection.getRangeAt(0);
  return {
    context: ["http://www.w3.org/ns/anno.jsonld"],
    motivation: ["HIGHLIGHTING"],
    type: "ANNOTATION",
    target: [
      {
        value: selection.toString(),
        format: "TEXT_PLAIN",
        language: LANGUAGE,
        processingLanguage: LANGUAGE,
        textDirection: TEXT_DIRECTION,
      },
      {
        source: {
          id: window.location.href,
          format: "TEXT_HTML",
          language: LANGUAGE,
          processingLanguage: LANGUAGE,
          textDirection: TEXT_DIRECTION,
          type: "TEXT",
        },
        selector: [
          {
            type: "RANGE_SELECTOR",
            startSelector: makeXPathSelectorFromRange({
              container: range.startContainer,
              startPosition: range.startOffset,
              endPosition: range.startContainer.textContent.length,
            }),
            endSelector: makeXPathSelectorFromRange({
              container: range.endContainer,
              startPosition: 0,
              endPosition: range.endOffset,
            }),
          },
        ],
      },
    ],
  };
};

;// CONCATENATED MODULE: ./get-annotation/index.mjs


/* harmony default export */ const get_annotation = (() => {
  const output = !window.getSelection().empty()
    ? makeAnnotationFromSelection({ selection: window.getSelection() })
    : null;

  window.getSelection().removeAllRanges();

  return output;
});

Literal = __webpack_exports__;
/******/ })()
;
 Literal.default();