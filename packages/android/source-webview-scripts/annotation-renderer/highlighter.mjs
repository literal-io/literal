// adapted from https://gist.github.com/Gozala/80cf4d2c9f000548b7a11b110b1d7711
export class Highlighter {
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
    return this.isHighlightableText(node) || this.isHighlightableImage(node);
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

  removeHighlight(annotationId) {
    document
      .querySelectorAll(
        `.${this.highlightClassName}[data-annotation-id="${annotationId}"]`
      )
      .forEach((elem) => {
        const parentNode = elem.parentNode;
        elem.replaceWith(...Array.from(elem.childNodes));
        parentNode.normalize();
      });
  }

  removeHighlights() {
    document.querySelectorAll(`.${this.highlightClassName}`).forEach((elem) => {
      const parentNode = elem.parentNode;
      elem.replaceWith(...Array.from(elem.childNodes));
      parentNode.normalize();
    });
  }
}
