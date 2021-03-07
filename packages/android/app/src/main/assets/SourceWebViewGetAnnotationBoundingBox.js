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
  "default": () => (/* binding */ get_annotation_bounding_box)
});

;// CONCATENATED MODULE: ./shared/storage.mjs
const NAMESPACE = "__literal";

const initialize = () => {
  window[NAMESPACE] = window[NAMESPACE] || {};
};

const set = (key, value) => {
  window[NAMESPACE][key] = value;
};

const get = (key) => window[NAMESPACE][key];

;// CONCATENATED MODULE: ./get-annotation-bounding-box/index.mjs

const ANNOTATION = process.env.PARAM_ANNOTATION;

/* harmony default export */ const get_annotation_bounding_box = (() => {
  initialize();

  const ranges = get("annotationRanges");
  if (!ranges || !ranges[ANNOTATION.id]) {
    console.error("[Literal] Unable to find range for annotation.");
    return;
  }

  const boundingBox = ranges[ANNOTATION.id].getBoundingClientRect();
  return {
    left: boundingBox.left,
    top: boundingBox.top,
    right: boundingBox.right,
    bottom: boundingBox.bottom,
  };
});

Literal = __webpack_exports__;
/******/ })()
;
 Literal.default();