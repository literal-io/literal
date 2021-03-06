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
        annotationId: message.data.annotationId,
        disableNotify: true,
      });
    }
  }

  onAnnotationsRendered() {
    // focus on annotation click
    document.querySelectorAll(`.${this.highlightClassName}`).forEach((el) => {
      el.addEventListener("click", (ev) => {
        ev.preventDefault();
        ev.stopPropagation();

        this._handleFocusAnnotation({
          annotationId: ev.target.getAttribute("data-annotation-id"),
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

  _handleFocusAnnotation({ annotationId, disableNotify }) {
    if (this.focusedAnnotationId === annotationId) {
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
    if (!this.focusedAnnotationElems) {
      console.error(
        `[Literal] Could not find elements for annotation: ${annotationId}`
      );
      return null;
    }

    this.focusedAnnotationElemIsVisible = new Map(
      this.focusedAnnotationElems.map((elem) => [elem, false])
    );
    this.focusedAnnotationElems.forEach((elem) => {
      this.focusedAnnotationIntersectionObserver.observe(elem);
    });

    if (!disableNotify) {
      this.messenger.postMessage({
        type: "FOCUS_ANNOTATION",
        data: {
          annotationId,
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
