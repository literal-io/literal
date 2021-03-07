import {
  evaluate as evaluateXPath,
  xPathRangeSelectorPredicate,
} from "../shared/xpath.mjs";
import { get as storageGet } from "../shared/storage.mjs";

export class AnnotationFocusManager {
  constructor({ messenger, highlightClassName }) {
    this.messenger = messenger;
    this.highlightClassName = highlightClassName;
    this.eventQueue = [];
    this.annotationsRendered = false;

    this.focusedAnnotationIntersectionObserver = new IntersectionObserver(
      this._handleIntersectionObserverEntries,
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

    if (Array.from(entries.values()).every((isVisible) => !isVisible)) {
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

    const startNode = evaluateXPath(targetRangeSelector.startSelector.value);
    const endNode = evaluateXPath(targetRangeSelector.endSelector.value);

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
      if (scrollIntoView) {
        this.focusedAnnotationElems[0].scrollIntoView({
          behavior: "auto",
          block: "center",
          inline: "center",
        });

        if (!disableNotify) {
          const ranges = storageGet("annotationRanges");
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
      const ranges = storageGet("annotationRanges");
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
