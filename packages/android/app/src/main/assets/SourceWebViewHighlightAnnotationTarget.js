(function highlightSelectors() {
  const HIGHLIGHT_CLASS_NAME = "literal-highlight";
  const ANNOTATIONS = JSON.parse('${PARAM_ANNOTATIONS}');

  // adapted from https://gist.github.com/Gozala/80cf4d2c9f000548b7a11b110b1d7711
  const Highlighter = (function() {
    const markText = (text) => {
      const span = document.createElement("span");
      span.role = "mark";
      span.style.backgroundColor = "rgb(0, 0, 0)";
      span.style.color = "rgba(255, 255, 255, 0.92)";
      span.style.display = "inline";
      span.classList.add(HIGHLIGHT_CLASS_NAME);
      text.parentNode.replaceChild(span, text);
      span.appendChild(text);
      return span;
    };

    const markImage = (image) => {
      const selected = image.cloneNode();
      selected.role = "mark";
      selected.style.objectPosition = `${image.width}px`;
      selected.style.backgroundImage = `url(${image.src})`;
      selected.style.backgroundColor = "rgba(255, 255, 0, 0.3)";
      selected.style.backgroundBlendMode = "overlay";
      selected.classList.add(HIGHLIGHT_CLASS_NAME);
      // Keep original node so we can remove highlighting by
      // swapping back images.
      image.appendChild(selected);

      image.parentElement.replaceChild(selected, image);

      return selected;
    };

    const markNode = (node) => {
      const { Image, Text } = node.ownerDocument.defaultView;
      if (node instanceof Image) {
        return markImage(node);
      } else if (node instanceof Text) {
        return markText(node);
      } else {
        return node;
      }
    };

    const filter = function*(p, iterator) {
      for (let item of iterator) {
        if (p(item)) {
          yield item;
        }
      }
    };

    const takeWhile = function*(p, iterator) {
      for (let item of iterator) {
        if (p(item)) {
          yield item;
        } else {
          break;
        }
      }
    };

    const nextNodes = function*(node) {
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

    const resolveContainer = (node, offset) => {
      const { Text } = node.ownerDocument.defaultView;
      const result =
        node instanceof Text
          ? [node, offset]
          : offset < node.childNodes.length
          ? [node.childNodes[offset], 0]
          : Error("No child matching the offset found");
      return result;
    };

    const highlightTextRange = (text, startOffset, endOffset) => {
      const prefix = text;
      const content = text.splitText(startOffset);
      const suffix = content.splitText(endOffset - startOffset);
      return [prefix, content, suffix];
    };

    const isHighlightableNode = (node) =>
      isHighlightableText(node) || isHighlightableImage(node);

    const isHighlightableText = (node) =>
      node instanceof node.ownerDocument.defaultView.Text &&
      node.textContent.trim().length > 0;

    const isHighlightableImage = (node) =>
      node instanceof node.ownerDocument.defaultView.Image;

    const highlightRange = (range) => {
      const { startContainer, endContainer, startOffset, endOffset } = range;
      const start = resolveContainer(startContainer, startOffset);
      const end = resolveContainer(endContainer, endOffset);

      if (start instanceof Error) {
        return Error(`Invalid start of the range: ${start}`);
      } else if (end instanceof Error) {
        return Error(`Invalid end of the range: ${end}`);
      } else {
        const { Text } = startContainer.ownerDocument.defaultView;
        const [startNode, startOffset] = start;
        const [endNode, endOffset] = end;

        if (startNode === endNode && startNode instanceof Text) {
          const [previous, text, next] = highlightTextRange(
            startNode,
            startOffset,
            endOffset
          );
          markText(text);
          range.setStart(text, 0);
          range.setEnd(next, 0);
        } else {
          const contentNodes = takeWhile(
            (node) => node !== endNode,
            nextNodes(startNode)
          );
          const highlightableNodes = filter(isHighlightableNode, contentNodes);

          [...highlightableNodes].forEach(markNode);

          if (startNode instanceof Text) {
            const text =
              startOffset > 0 ? startNode.splitText(startOffset) : startNode;

            markText(text);
            range.setStart(text, 0);
          }

          if (endNode instanceof Text) {
            const [text, offset] =
              endOffset < endNode.length
                ? [endNode.splitText(endOffset).previousSibling, 0]
                : [endNode, endOffset];

            markText(text);
            range.setEnd(text, text.length);
          }
        }
      }
    };

    return { highlightRange };
  })();

  // remove any existing selectors
  document.querySelectorAll(`.${HIGHLIGHT_CLASS_NAME}`).forEach((elem) => {
    const parentNode = elem.parentNode;
    elem.replaceWith(...Array.from(elem.childNodes));
    parentNode.normalize();
  });

  ANNOTATIONS.reduce((rangeSelectors, annotation) => {
    if (annotation.target) {
      return annotation.target
        .filter(({ type }) => type === "SPECIFIC_RESOURCE")
        .map(({ selector }) =>
          selector.filter(({ type }) => type === "RANGE_SELECTOR")
        )
        .flat()
        .concat(rangeSelectors);
    }
    return rangeSelectors;
  }, []).forEach(({ startSelector, endSelector }) => {
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
      Highlighter.highlightRange(range);
    } catch (e) {
      /** noop **/
    }
  });
})();
