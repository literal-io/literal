var Literal;
/******/ (() => { // webpackBootstrap
/******/ 	"use strict";
/******/ 	var __webpack_modules__ = ({

/***/ 345:
/***/ ((module) => {


module.exports = (promise, onFinally) => {
	onFinally = onFinally || (() => {});

	return promise.then(
		val => new Promise(resolve => {
			resolve(onFinally());
		}).then(() => val),
		err => new Promise(resolve => {
			resolve(onFinally());
		}).then(() => {
			throw err;
		})
	);
};


/***/ }),

/***/ 147:
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {



const pFinally = __webpack_require__(345);

class TimeoutError extends Error {
	constructor(message) {
		super(message);
		this.name = 'TimeoutError';
	}
}

const pTimeout = (promise, milliseconds, fallback) => new Promise((resolve, reject) => {
	if (typeof milliseconds !== 'number' || milliseconds < 0) {
		throw new TypeError('Expected `milliseconds` to be a positive number');
	}

	if (milliseconds === Infinity) {
		resolve(promise);
		return;
	}

	const timer = setTimeout(() => {
		if (typeof fallback === 'function') {
			try {
				resolve(fallback());
			} catch (error) {
				reject(error);
			}

			return;
		}

		const message = typeof fallback === 'string' ? fallback : `Promise timed out after ${milliseconds} milliseconds`;
		const timeoutError = fallback instanceof Error ? fallback : new TimeoutError(message);

		if (typeof promise.cancel === 'function') {
			promise.cancel();
		}

		reject(timeoutError);
	}, milliseconds);

	// TODO: Use native `finally` keyword when targeting Node.js 10
	pFinally(
		// eslint-disable-next-line promise/prefer-await-to-then
		promise.then(resolve, reject),
		() => {
			clearTimeout(timer);
		}
	);
});

module.exports = pTimeout;
// TODO: Remove this for the next major release
module.exports.default = pTimeout;

module.exports.TimeoutError = TimeoutError;


/***/ }),

/***/ 299:
/***/ ((module, __unused_webpack_exports, __webpack_require__) => {


const pTimeout = __webpack_require__(147);

const pWaitFor = async (condition, options) => {
	options = {
		interval: 20,
		timeout: Infinity,
		leadingCheck: true,
		...options
	};

	let retryTimeout;

	const promise = new Promise((resolve, reject) => {
		const check = async () => {
			try {
				const value = await condition();

				if (typeof value !== 'boolean') {
					throw new TypeError('Expected condition to return a boolean');
				}

				if (value === true) {
					resolve();
				} else {
					retryTimeout = setTimeout(check, options.interval);
				}
			} catch (error) {
				reject(error);
			}
		};

		if (options.leadingCheck) {
			check();
		} else {
			retryTimeout = setTimeout(check, options.interval);
		}
	});

	if (options.timeout !== Infinity) {
		try {
			return await pTimeout(promise, options.timeout);
		} catch (error) {
			if (retryTimeout) {
				clearTimeout(retryTimeout);
			}

			throw error;
		}
	}

	return promise;
};

module.exports = pWaitFor;
// TODO: Remove this for the next major release
module.exports.default = pWaitFor;


/***/ })

/******/ 	});
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		if(__webpack_module_cache__[moduleId]) {
/******/ 			return __webpack_module_cache__[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			// no module.id needed
/******/ 			// no module.loaded needed
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId](module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
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
// This entry need to be wrapped in an IIFE because it need to be isolated against other modules in the chunk.
(() => {
// ESM COMPAT FLAG
__webpack_require__.r(__webpack_exports__);

// EXPORTS
__webpack_require__.d(__webpack_exports__, {
  "default": () => (/* binding */ highlight_annotation_target)
});

;// CONCATENATED MODULE: ./highlight-annotation-target/messenger.mjs
class Messenger {
  constructor() {
    this.handlers = new Map();

    window.addEventListener("message", (ev) => {
      if (ev.ports && ev.ports.length > 0 && !globalThis.literalMessagePort) {
        globalThis.literalMessagePort = ev.ports[0];
      }

      this._handleMessage(ev);
    });
  }

  _handleMessage(ev) {
    try {
      const data = JSON.parse(ev.data);

      console.log("[Literal] Receieved message", ev.data);
      if (this.handlers.has(data.type)) {
        this.handlers.get(data.type).forEach((handler) => handler(data));
      }
    } catch (e) {
      console.error("[Literal] Unable to parse message", e, ev.data);
    }
  }

  postMessage(ev) {
    if (!globalThis.literalMessagePort) {
      console.error("[Literal] Unable to dispatch: has not initialized");
      return;
    }
    globalThis.literalMessagePort.postMessage(JSON.stringify(ev));
  }

  on(type, handler) {
    if (!this.handlers.has(type)) {
      this.handlers.set(type, [handler]);
    } else {
      this.handlers.get(type).push(handler);
    }
  }
}

;// CONCATENATED MODULE: ./highlight-annotation-target/highlighter.mjs
// adapted from https://gist.github.com/Gozala/80cf4d2c9f000548b7a11b110b1d7711
class Highlighter {
  constructor({ highlightClassName }) {
    this.highlightClassName = highlightClassName;
  }

  markText(text, dataset) {
    const span = document.createElement("span");

    span.role = "mark";
    span.style.backgroundColor = "rgb(0, 0, 0)";
    span.style.color = "rgba(255, 255, 255, 0.92)";
    span.style.display = "inline";
    span.style.userSelect = "none";

    span.classList.add(this.highlightClassName);
    text.parentNode.replaceChild(span, text);
    Object.keys(dataset).forEach((key) => {
      span.setAttribute(`data-${key}`, dataset[key]);
    });
    span.appendChild(text);
    return span;
  }

  markImage(image, dataset) {
    const selected = image.cloneNode();

    selected.role = "mark";
    selected.style.objectPosition = `${image.width}px`;
    selected.style.backgroundImage = `url(${image.src})`;
    selected.style.backgroundColor = "rgba(255, 255, 0, 0.3)";
    selected.style.backgroundBlendMode = "overlay";
    span.style.userSelect = "none";
    selected.classList.add(this.highlightClassName);

    Object.keys(dataset).forEach((key) => {
      selected.setAttribute(`data-${key}`, dataset[key]);
    });
    // Keep original node so we can remove highlighting by
    // swapping back images.
    image.appendChild(selected);

    image.parentElement.replaceChild(selected, image);

    return selected;
  }

  markNode(node, dataset) {
    const { Image, Text } = node.ownerDocument.defaultView;
    if (node instanceof Image) {
      return this.markImage(node, dataset);
    } else if (node instanceof Text) {
      return this.markText(node, dataset);
    } else {
      return node;
    }
  }

  *filter(p, iterator) {
    for (let item of iterator) {
      if (p(item)) {
        yield item;
      }
    }
  }

  *takeWhile(p, iterator) {
    for (let item of iterator) {
      if (p(item)) {
        yield item;
      } else {
        break;
      }
    }
  }

  *nextNodes(node) {
    let next = node;
    let isWalkingUp = false;
    while (next != null) {
      if (!isWalkingUp && next.firstChild != null) {
        [isWalkingUp, next] = [false, next.firstChild];
        yield next;
      } else if (next.nextSibling != null) {
        [isWalkingUp, next] = [false, next.nextSibling];
        yield next;
      } else {
        [isWalkingUp, next] = [true, next.parentNode];
      }
    }
  }

  resolveContainer(node, offset) {
    const { Text } = node.ownerDocument.defaultView;
    const result =
      node instanceof Text
        ? [node, offset]
        : offset < node.childNodes.length
        ? [node.childNodes[offset], 0]
        : Error("No child matching the offset found");
    return result;
  }

  highlightTextRange(text, startOffset, endOffset) {
    const prefix = text;
    const content = text.splitText(startOffset);
    const suffix = content.splitText(endOffset - startOffset);
    return [prefix, content, suffix];
  }

  isHighlightableNode(node) {
    return (
      this.isHighlightableText(node) ||
      this.isHighlightableImage(node)
    );
  }

  isHighlightableText(node) {
    return (
      node instanceof node.ownerDocument.defaultView.Text &&
      node.textContent.trim().length > 0
    );
  }

  isHighlightableImage(node) {
    return node instanceof node.ownerDocument.defaultView.Image;
  }

  highlightRange(range, markDataset) {
    const { startContainer, endContainer, startOffset, endOffset } = range;
    const start = this.resolveContainer(startContainer, startOffset);
    const end = this.resolveContainer(endContainer, endOffset);

    if (start instanceof Error) {
      return Error(`Invalid start of the range: ${start}`);
    } else if (end instanceof Error) {
      return Error(`Invalid end of the range: ${end}`);
    } else {
      const { Text } = startContainer.ownerDocument.defaultView;
      const [startNode, startOffset] = start;
      const [endNode, endOffset] = end;

      if (startNode === endNode && startNode instanceof Text) {
        const [previous, text, next] = this.highlightTextRange(
          startNode,
          startOffset,
          endOffset
        );
        this.markText(text, markDataset);
        range.setStart(text, 0);
        range.setEnd(next, 0);
      } else {
        const contentNodes = this.takeWhile(
          (node) => node !== endNode,
          this.nextNodes(startNode)
        );
        const highlightableNodes = this.filter(
          (node) => this.isHighlightableNode(node),
          contentNodes
        );

        [...highlightableNodes].forEach((node) =>
          this.markNode(node, markDataset)
        );

        if (startNode instanceof Text) {
          const text =
            startOffset > 0 ? startNode.splitText(startOffset) : startNode;

          this.markText(text, markDataset);
          range.setStart(text, 0);
        }

        if (endNode instanceof Text) {
          const [text, offset] =
            endOffset < endNode.length
              ? [endNode.splitText(endOffset).previousSibling, 0]
              : [endNode, endOffset];

          this.markText(text, markDataset);
          range.setEnd(text, text.length);
        }
      }
    }
  }

  removeHighlights() {
    document.querySelectorAll(`.${this.highlightClassName}`).forEach((elem) => {
      const parentNode = elem.parentNode;
      elem.replaceWith(...Array.from(elem.childNodes));
      parentNode.normalize();
    });
  }
}

;// CONCATENATED MODULE: ./shared/xpath.mjs
const evaluate = (value) =>
  document.evaluate(
    value,
    document,
    null,
    XPathResult.FIRST_ORDERED_NODE_TYPE,
    null
  ).singleNodeValue;

const xPathRangeSelectorPredicate = ({
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

;// CONCATENATED MODULE: ./shared/storage.mjs
const NAMESPACE = "__literal";

const initialize = () => {
  window[NAMESPACE] = window[NAMESPACE] || {};
};

const set = (key, value) => {
  window[NAMESPACE][key] = value;
};

const get = (key) => window[NAMESPACE][key];

;// CONCATENATED MODULE: ./highlight-annotation-target/annotation-focus-manager.mjs



class AnnotationFocusManager {
  constructor({ messenger, highlightClassName }) {
    this.messenger = messenger;
    this.highlightClassName = highlightClassName;
    this.eventQueue = [];
    this.annotationsRendered = false;

    this.focusedAnnotationIntersectionObserver = new IntersectionObserver(
      (entries) => this._handleIntersectionObserverEntries(entries),
      {
        root: null,
        rootMargin: "0px",
        threshold: [0, 1],
      }
    );

    this.messenger.on("FOCUS_ANNOTATION", (message) => {
      this.handleEvent({ type: "FOCUS_ANNOTATION", data: message.data });
    });
  }

  handleEvent({ type, data }) {
    if (!this.annotationsRendered) {
      this.eventQueue.push({ type, data });
      return;
    }

    if (type === "FOCUS_ANNOTATION") {
      if (!data.annotationId) {
        console.error(
          "[Literal] Received FOCUS_ANNOTATION event without annotationId."
        );
        return;
      }

      this._handleFocusAnnotation({
        annotationId: data.annotationId,
        scrollIntoView: true,
        disableNotify: true,
      });
    }
  }

  onAnnotationsRendered(annotations) {
    this.annotations = annotations;

    // focus on annotation click
    document.querySelectorAll(`.${this.highlightClassName}`).forEach((el) => {
      el.addEventListener("click", (ev) => {
        ev.preventDefault();
        ev.stopPropagation();

        this._handleFocusAnnotation({
          annotationId: ev.target.getAttribute("data-annotation-id"),
          scrollIntoView: true,
        });
      });
    });

    // blur on body click
    document.body.addEventListener("click", (_ev) => {
      if (!this.focusedAnnotationElems) {
        return;
      }

      this._handleBlurAnnotation();
    });

    this.annotationsRendered = true;
    this.eventQueue.forEach((event) => this.handleEvent(event));
    this.eventQueue = [];
  }

  _handleIntersectionObserverEntries(entries) {
    entries.forEach((entry) => {
      if (!this.focusedAnnotationElemIsVisible) {
        console.error(`[Literal] Could not set visibility for entry: ${entry}`);
        return;
      }

      this.focusedAnnotationElemIsVisible.set(
        entry.target,
        entry.isIntersecting
      );
    });

    if (
      Array.from(this.focusedAnnotationElemIsVisible.values()).every(
        (isVisible) => !isVisible
      )
    ) {
      this._handleBlurAnnotation();
    }
  }

  _handleEditAnnotationTarget({ annotationId }) {
    const annotation = this.annotations.find(
      (annotation) => annotation.id === annotationId
    );
    if (!annotation) {
      console.error(
        "[Literal] Unable to find annotation.",
        annotationId,
        this.annotations
      );
      return;
    }

    const target = annotation.target.find(
      (target) =>
        target.type === "SPECIFIC_RESOURCE" &&
        (target.selector || []).some(xPathRangeSelectorPredicate)
    );
    if (!target) {
      console.error(
        "[Literal] Unable to find supported long press target.",
        annotation
      );
      return;
    }

    const targetRangeSelector = target.selector.find(
      ({ type }) => type === "RANGE_SELECTOR"
    );
    const startTextPositionSelector = targetRangeSelector.startSelector.refinedBy.find(
      ({ type }) => type === "TEXT_POSITION_SELECTOR"
    );
    const endTextPositionSelector = targetRangeSelector.endSelector.refinedBy.find(
      ({ type }) => type === "TEXT_POSITION_SELECTOR"
    );

    const startNode = evaluate(targetRangeSelector.startSelector.value);
    const endNode = evaluate(targetRangeSelector.endSelector.value);

    const range = document.createRange();
    range.setStart(startNode, startTextPositionSelector.start);
    range.setEnd(endNode, endTextPositionSelector.end);

    window.getSelection().removeAllRanges();
    window.getSelection().addRange(range);

    this.messenger.postMessage({
      type: "EDIT_ANNOTATION_TARGET",
      data: {
        annotationId,
        targetId,
      },
    });
  }

  _handleFocusAnnotation({ annotationId, disableNotify, scrollIntoView }) {
    if (this.focusedAnnotationId === annotationId) {
      const isVisible = Array.from(
        this.focusedAnnotationElemIsVisible.values()
      ).some((isVisible) => isVisible);

      if (scrollIntoView && !isVisible) {
        this.focusedAnnotationElems[0].scrollIntoView({
          behavior: "auto",
          block: "center",
          inline: "center",
        });
      }

      if (!disableNotify) {
        const ranges = get("annotationRanges");
        if (!ranges || !ranges[annotationId]) {
          console.error("[Literal] Unable to find range for annotation");
          return;
        }

        const boundingBox = ranges[annotationId].getBoundingClientRect();
        this.messenger.postMessage({
          type: "FOCUS_ANNOTATION",
          data: {
            annotationId,
            boundingBox: {
              left: boundingBox.left * window.devicePixelRatio,
              top: boundingBox.top * window.devicePixelRatio,
              right: boundingBox.right * window.devicePixelRatio,
              bottom: boundingBox.bottom * window.devicePixelRatio,
            },
          },
        });
      }
      return;
    }

    if (this.focusedAnnotationElems) {
      this._handleBlurAnnotation();
    }

    this.focusedAnnotationId = annotationId;
    this.focusedAnnotationElems = Array.from(
      document.querySelectorAll(
        `.${this.highlightClassName}[data-annotation-id="${annotationId}"]`
      )
    );
    if (
      !this.focusedAnnotationElems ||
      this.focusedAnnotationElems.length === 0
    ) {
      console.error(
        `[Literal] Could not find elements for annotation: ${annotationId}`
      );
      return null;
    }

    if (scrollIntoView) {
      this.focusedAnnotationElems[0].scrollIntoView({
        behavior: "auto",
        block: "center",
        inline: "center",
      });
    }

    this.focusedAnnotationElemIsVisible = new Map(
      this.focusedAnnotationElems.map((elem) => [elem, false])
    );
    this.focusedAnnotationElems.forEach((elem) => {
      this.focusedAnnotationIntersectionObserver.observe(elem);
    });

    if (!disableNotify) {
      const ranges = get("annotationRanges");
      if (!ranges || !ranges[annotationId]) {
        console.error("[Literal] Unable to find range for annotation");
        return;
      }

      const boundingBox = ranges[annotationId].getBoundingClientRect();
      this.messenger.postMessage({
        type: "FOCUS_ANNOTATION",
        data: {
          annotationId,
          boundingBox: {
            left: boundingBox.left * window.devicePixelRatio,
            top: boundingBox.top * window.devicePixelRatio,
            right: boundingBox.right * window.devicePixelRatio,
            bottom: boundingBox.bottom * window.devicePixelRatio,
          },
        },
      });
    }
  }

  _handleBlurAnnotation() {
    console.log("handleBlurAnnotation");
    if (!this.focusedAnnotationElems) {
      console.warn(
        `[Literal] Call to handleBlurAnnotation without a focused annotation.`
      );
      return;
    }

    this.focusedAnnotationElemIsVisible = null;
    this.focusedAnnotationElems.forEach((elem) => {
      this.focusedAnnotationIntersectionObserver.unobserve(elem);
    });
    this.focusedAnnotationElems = null;
    this.focusedAnnotationId = null;

    this.messenger.postMessage({
      type: "BLUR_ANNOTATION",
    });
  }
}

// EXTERNAL MODULE: ./node_modules/p-wait-for/index.js
var p_wait_for = __webpack_require__(299);
;// CONCATENATED MODULE: ./highlight-annotation-target/index.mjs








const HIGHLIGHT_CLASS_NAME = "literal-highlight";
const ANNOTATIONS = process.env.PARAM_ANNOTATIONS;

const messenger = new Messenger({
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const annotationFocusManager = new AnnotationFocusManager({
  messenger,
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const highlighter = new Highlighter({
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});

const onDocumentReady = (cb) => {
  if (document.readyState === "complete") {
    cb();
  } else {
    document.addEventListener("DOMContentLoaded", () => {
      cb();
    });
  }
};

const waitForXPath = (value) => {
  return p_wait_for(
    () => {
      try {
        return Boolean(evaluate(value));
      } catch (err) {
        return false;
      }
    },
    { interval: 50, timeout: 5000 }
  ).then(() => evaluate(value));
};

/* harmony default export */ const highlight_annotation_target = (() =>
  onDocumentReady(async () => {
    initialize();
    highlighter.removeHighlights();
    set("annotationRanges", {});

    await Promise.all(
      ANNOTATIONS.reduce((rangeSelectors, annotation) => {
        if (annotation.target) {
          return annotation.target
            .filter(
              (target) =>
                target.type === "SPECIFIC_RESOURCE" &&
                (target.selector || []).some(xPathRangeSelectorPredicate)
            )
            .map(({ selector }) => selector.filter(xPathRangeSelectorPredicate))
            .flat()
            .map((selector) => ({ ...selector, annotationId: annotation.id }))
            .concat(rangeSelectors);
        }
        return rangeSelectors;
      }, []).map(async ({ startSelector, endSelector, annotationId }) => {
        const startTextPositionSelector = startSelector.refinedBy.find(
          ({ type }) => type === "TEXT_POSITION_SELECTOR"
        );
        const endTextPositionSelector = endSelector.refinedBy.find(
          ({ type }) => type === "TEXT_POSITION_SELECTOR"
        );
        const [startNode, endNode] = await Promise.all([
          waitForXPath(startSelector.value),
          waitForXPath(endSelector.value),
        ]);

        const range = document.createRange();
        range.setStart(startNode, startTextPositionSelector.start);
        range.setEnd(endNode, endTextPositionSelector.end);

        try {
          highlighter.highlightRange(range, { "annotation-id": annotationId });
          set("annotationRanges", {
            ...get("annotationRanges"),
            [annotationId]: range,
          });
        } catch (e) {
          console.error("[Literal] Unable to highlight range.", range, e);
        }
      })
    );

    annotationFocusManager.onAnnotationsRendered(ANNOTATIONS);
  }));

})();

Literal = __webpack_exports__;
/******/ })()
;
 Literal.default();