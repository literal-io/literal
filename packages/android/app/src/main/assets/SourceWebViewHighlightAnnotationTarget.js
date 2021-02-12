(function highlightAnnotationTarget() {
  const HIGHLIGHT_CLASS_NAME = "literal-highlight";
  const ANNOTATIONS = ${PARAM_ANNOTATIONS};

  class Messenger {

    handlers = new Map() 

    constructor() {
      window.addEventListener("message", (ev) => {
        if (ev.ports && ev.ports.length > 0 && !globalThis.literalMessagePort) {
          globalThis.literalMessagePort = ev.ports[0];
        }

        this._handleMessage(ev);
      });
    }

    _handleMessage = (ev) => {
      try {
        const data = JSON.parse(ev.data)

        if (this.handlers.has(data.type)) {
          this.handlers.get(data.type).forEach((handler) => handler(data))
        }
      } catch (e) {
        console.error("[Literal] Unable to parse message", e)
      }
    };

    postMessage = (ev) => {
      if (!globalThis.literalMessagePort) {
        console.error("[Literal] Unable to dispatch: has not initialized");
        return;
      }
      globalThis.literalMessagePort.postMessage(JSON.stringify(ev));
    };

    on = (type, handler) => {
      if (!this.handlers.has(type)) {
        this.handlers.set(type, [handler])
      } else {
        this.handlers.get(type).push(handler)
      }
    }
  }

  // adapted from https://gist.github.com/Gozala/80cf4d2c9f000548b7a11b110b1d7711
  class Highlighter {
    static markText = (text, dataset) => {
      const span = document.createElement("span");
      span.role = "mark";
      span.style.backgroundColor = "rgb(0, 0, 0)";
      span.style.color = "rgba(255, 255, 255, 0.92)";
      span.style.display = "inline";
      span.classList.add(HIGHLIGHT_CLASS_NAME);
      text.parentNode.replaceChild(span, text);
      Object.keys(dataset).forEach((key) => {
        span.setAttribute(`data-${key}`, dataset[key]);
      });
      span.appendChild(text);
      return span;
    };

    static markImage = (image, dataset) => {
      const selected = image.cloneNode();
      selected.role = "mark";
      selected.style.objectPosition = `${image.width}px`;
      selected.style.backgroundImage = `url(${image.src})`;
      selected.style.backgroundColor = "rgba(255, 255, 0, 0.3)";
      selected.style.backgroundBlendMode = "overlay";
      selected.classList.add(HIGHLIGHT_CLASS_NAME);
      Object.keys(dataset).forEach((key) => {
        selected.setAttribute(`data-${key}`, dataset[key]);
      });
      // Keep original node so we can remove highlighting by
      // swapping back images.
      image.appendChild(selected);

      image.parentElement.replaceChild(selected, image);

      return selected;
    };

    static markNode = (node, dataset) => {
      const { Image, Text } = node.ownerDocument.defaultView;
      if (node instanceof Image) {
        return Highlighter.markImage(node, dataset);
      } else if (node instanceof Text) {
        return Highlighter.markText(node, dataset);
      } else {
        return node;
      }
    };

    static filter = function*(p, iterator) {
      for (let item of iterator) {
        if (p(item)) {
          yield item;
        }
      }
    };

    static takeWhile = function*(p, iterator) {
      for (let item of iterator) {
        if (p(item)) {
          yield item;
        } else {
          break;
        }
      }
    };

    static nextNodes = function*(node) {
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
    };

    static resolveContainer = (node, offset) => {
      const { Text } = node.ownerDocument.defaultView;
      const result =
        node instanceof Text
          ? [node, offset]
          : offset < node.childNodes.length
          ? [node.childNodes[offset], 0]
          : Error("No child matching the offset found");
      return result;
    };

    static highlightTextRange = (text, startOffset, endOffset) => {
      const prefix = text;
      const content = text.splitText(startOffset);
      const suffix = content.splitText(endOffset - startOffset);
      return [prefix, content, suffix];
    };

    static isHighlightableNode = (node) =>
      Highlighter.isHighlightableText(node) || Highlighter.isHighlightableImage(node);

    static isHighlightableText = (node) =>
      node instanceof node.ownerDocument.defaultView.Text &&
      node.textContent.trim().length > 0;

    static isHighlightableImage = (node) =>
      node instanceof node.ownerDocument.defaultView.Image;

    static highlightRange = (range, markDataset) => {
      const { startContainer, endContainer, startOffset, endOffset } = range;
      const start = Highlighter.resolveContainer(startContainer, startOffset);
      const end = Highlighter.resolveContainer(endContainer, endOffset);

      if (start instanceof Error) {
        return Error(`Invalid start of the range: ${start}`);
      } else if (end instanceof Error) {
        return Error(`Invalid end of the range: ${end}`);
      } else {
        const { Text } = startContainer.ownerDocument.defaultView;
        const [startNode, startOffset] = start;
        const [endNode, endOffset] = end;

        if (startNode === endNode && startNode instanceof Text) {
          const [previous, text, next] = Highlighter.highlightTextRange(
            startNode,
            startOffset,
            endOffset
          );
          Highlighter.markText(text, markDataset);
          range.setStart(text, 0);
          range.setEnd(next, 0);
        } else {
          const contentNodes = Highlighter.takeWhile(
            (node) => node !== endNode,
            Highlighter.nextNodes(startNode)
          );
          const highlightableNodes = Highlighter.filter(Highlighter.isHighlightableNode, contentNodes);

          [...highlightableNodes].forEach((node) =>
            Highlighter.markNode(node, markDataset)
          );

          if (startNode instanceof Text) {
            const text =
              startOffset > 0 ? startNode.splitText(startOffset) : startNode;

            Highlighter.markText(text, markDataset);
            range.setStart(text, 0);
          }

          if (endNode instanceof Text) {
            const [text, offset] =
              endOffset < endNode.length
                ? [endNode.splitText(endOffset).previousSibling, 0]
                : [endNode, endOffset];

            Highlighter.markText(text, markDataset);
            range.setEnd(text, text.length);
          }
        }
      }
    };

    static removeHighlights = () => {
      document.querySelectorAll(`.${HIGHLIGHT_CLASS_NAME}`).forEach((elem) => {
        const parentNode = elem.parentNode;
        elem.replaceWith(...Array.from(elem.childNodes));
        parentNode.normalize();
      });
    };
  }

  class AnnotationFocusManager {
    focusedAnnotationElems = null;
    focusedAnnotationIntersectionObserver = null;
    focusedAnnotationElemIsVisible = null;
    messenger = null;

    constructor({ messenger }) {
      this.messenger = messenger;
      this.focusedAnnotationIntersectionObserver = new IntersectionObserver(
        this._handleIntersectionObserverEntries,
        {
          root: null,
          rootMargin: "0px",
          threshold: [0, 1],
        }
      );

      this.messenger.on("ANNOTATION_FOCUS", (data) => {
        if (!data.annotationId) {
          console.error(
            "[Literal] Received ANNOTATION_FOCUS event without annotationId."
          );
          return;
        }

        this._handleFocusAnnotation({
          annotationId: data.annotationId,
          disableNotify: true,
        });
      });
    }

    registerClickListeners() {
      // focus on annotation click
      document.querySelectorAll(`.${HIGHLIGHT_CLASS_NAME}`).forEach((el) => {
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

        this.handleBlurAnnotation();
      });
    }

    _handleIntersectionObserverEntries(entries) {
      entries.forEach((entry) => {
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
      if (this.focusedAnnotationElems) {
        this.handleBlurAnnotation();
      }

      this.focusedAnnotationElems = Array.from(document.querySelectorAll(
        `.${HIGHLIGHT_CLASS_NAME}[data-annotation-id="${annotationId}"]`
      ));
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

      this.messenger.postMessage({
        type: "BLUR_ANNOTATION",
      });
    }
  }

  const messenger = new Messenger();
  const annotationFocusManager = new AnnotationFocusManager({ messenger });

  Highlighter.removeHighlights();

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
  }, []).forEach(({ startSelector, endSelector, annotationId }) => {
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

    try {
      Highlighter.highlightRange(range, { "annotation-id": annotationId });
    } catch (e) {
      console.error("[Literal] Unable to highlight range.", range, e);
    }
  });

  annotationFocusManager.registerClickListeners();
})();
