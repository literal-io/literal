import { Messenger } from "./messenger.mjs";
import { Highlighter } from "./highlighter.mjs";
import { AnnotationFocusManager } from "./annotation-focus-manager.mjs";
import waitFor from "p-wait-for";

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

const xPathEvaluate = (value) => {
  const evaluate = () =>
    document.evaluate(
      value,
      document,
      null,
      XPathResult.FIRST_ORDERED_NODE_TYPE,
      null
    ).singleNodeValue;

  return waitFor(
    () => {
      try {
        return Boolean(evaluate());
      } catch (err) {
        return false;
      }
    },
    { interval: 50, timeout: 5000 }
  ).then(() => evaluate());
};

(() =>
  onDocumentReady(async () => {
    highlighter.removeHighlights();

    await Promise.all(
      ANNOTATIONS.reduce((rangeSelectors, annotation) => {
        if (annotation.target) {
          return annotation.target
            .filter(({ type }) => type === "SPECIFIC_RESOURCE")
            .map(({ selector }) =>
              selector.filter(({ type }) => type === "RANGE_SELECTOR")
            )
            .flat()
            .map((selector) => ({ ...selector, annotationId: annotation.id }))
            .concat(rangeSelectors);
        }
        return rangeSelectors;
      }, []).map(async ({ startSelector, endSelector, annotationId }) => {
        const [startNode, endNode] = await Promise.all([
          xPathEvaluate(startSelector.value),
          xPathEvaluate(endSelector.value),
        ]);

        const range = document.createRange();
        range.setStart(startNode, startSelector.refinedBy[0].start);
        range.setEnd(endNode, endSelector.refinedBy[0].end);

        try {
          highlighter.highlightRange(range, { "annotation-id": annotationId });
        } catch (e) {
          console.error("[Literal] Unable to highlight range.", range, e);
        }
      })
    );

    annotationFocusManager.onAnnotationsRendered();
  }))();
