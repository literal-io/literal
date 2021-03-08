import { Messenger } from "./messenger.mjs";
import { Highlighter } from "./highlighter.mjs";
import { AnnotationFocusManager } from "./annotation-focus-manager.mjs";
import {
  evaluate as evaluateXPath,
  xPathRangeSelectorPredicate,
} from "../shared/xpath.mjs";
import {
  set as storageSet,
  get as storageGet,
  initialize as storageInitialize,
} from "../shared/storage.mjs";

import waitFor from "p-wait-for";

const HIGHLIGHT_CLASS_NAME = "literal-highlight";
const ANNOTATIONS = process.env.PARAM_ANNOTATIONS;

const messenger = new Messenger({
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const highlighter = new Highlighter({
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const annotationFocusManager = new AnnotationFocusManager({
  messenger,
  highlighter,
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
  return waitFor(
    () => {
      try {
        return Boolean(evaluateXPath(value));
      } catch (err) {
        return false;
      }
    },
    { interval: 50, timeout: 5000 }
  ).then(() => evaluateXPath(value));
};

export default () =>
  onDocumentReady(async () => {
    storageInitialize();
    highlighter.removeHighlights();
    storageSet("annotationRanges", {});

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
          storageSet("annotationRanges", {
            ...storageGet("annotationRanges"),
            [annotationId]: range,
          });
        } catch (e) {
          console.error("[Literal] Unable to highlight range.", range, e);
        }
      })
    );

    annotationFocusManager.onAnnotationsRendered(ANNOTATIONS);
  });
