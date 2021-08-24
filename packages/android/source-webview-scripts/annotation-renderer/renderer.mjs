import {
  evaluate as evaluateXPath,
  xPathRangeSelectorPredicate,
} from "../shared/xpath.mjs";

import { set as storageSet, get as storageGet } from "../shared/storage.mjs";
import waitFor from "p-wait-for";

const waitForXPath = (value) => {
  return waitFor(
    () => {
      try {
        return Boolean(evaluateXPath(value));
      } catch (err) {
        return false;
      }
    },
    { interval: 50, timeout: 5 * 1000 }
  ).then(() => evaluateXPath(value));
};

export class Renderer {
  constructor({ messenger, highlighter, annotationFocusManager }) {
    this.messenger = messenger;
    this.highlighter = highlighter;
    this.annotationFocusManager = annotationFocusManager;
    this.initialAnnotationsRendered = false;
    this.eventQueue = [];

    this.messenger.on("RENDER_ANNOTATIONS", (message) => {
      this.handleEvent({ type: "RENDER_ANNOTATIONS", data: message.data });
    });
  }

  onInitialAnnotationsRendered() {
    this.initialAnnotationsRendered = true;
    this.eventQueue.forEach((event) => this.handleEvent(event));
    this.eventQueue = [];
  }

  handleEvent({ type, data }) {
    if (!this.initialAnnotationsRendered) {
      this.eventQueue.push({ type, data });
      return;
    }

    if (type === "RENDER_ANNOTATIONS") {
      this.render(data.annotations).then(() => {
        this.annotationFocusManager.onAnnotationsRendered({
          annotations: data.annotations,
          focusedAnnotationId: data.focusedAnnotationId,
        });
      });
    }
  }

  render(annotations) {
    this.highlighter.removeHighlights();
    storageSet("annotationRanges", {});

    return Promise.all(
      annotations
        .reduce((rangeSelectors, annotation) => {
          if (annotation.target) {
            return annotation.target
              .filter(
                (target) =>
                  target.type === "SPECIFIC_RESOURCE" &&
                  (target.selector || []).some(xPathRangeSelectorPredicate)
              )
              .map(({ selector }) =>
                selector.filter(xPathRangeSelectorPredicate)
              )
              .flat()
              .map((selector) => ({ ...selector, annotationId: annotation.id }))
              .concat(rangeSelectors);
          }
          return rangeSelectors;
        }, [])
        .map(async ({ startSelector, endSelector, annotationId }) => {
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

          return { range, annotationId };
        })
    ).then((ranges) => {
      ranges.forEach(({ range, annotationId }) => {
        try {
          this.highlighter.highlightRange(range, {
            "annotation-id": annotationId,
          });
          storageSet("annotationRanges", {
            ...storageGet("annotationRanges"),
            [annotationId]: range,
          });
        } catch (e) {
          console.error("[Literal] Unable to highlight range.", range, e);
        }
      });

      return storageGet("annotationRanges");
    });
  }
}
