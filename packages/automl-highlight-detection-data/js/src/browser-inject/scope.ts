import { InjectScope, SelectionAnnotation, ViewportSize } from "./types";

export const scope: InjectScope = {
  getViewportSize: (): ViewportSize => {
    return {
      width: (window as any).visualViewport.width,
      height: (window as any).visualViewport.height,
      scale: (window as any).visualViewport.scale,
    };
  },
  getTextNodes: (el: HTMLElement): Text[] => {
    const walker = document.createTreeWalker(el, NodeFilter.SHOW_TEXT);
    const nodes = [];
    while (walker.nextNode()) {
      nodes.push(walker.currentNode as Text);
    }
    return nodes;
  },
  getRandomRange: (
    textNodes: Text[],
    boundaryAncestorSelector: string,
    { height: viewportHeight }: ViewportSize
  ): Range => {
    const startNodeIdx = Math.round(Math.random() * (textNodes.length - 1));
    const startNode = textNodes[startNodeIdx];

    const range = document.createRange();
    range.setStart(startNode, 0);

    // walk ranges of increasing distance until we exceed the viewport height
    let maxEndNodeIdx = Math.max(startNodeIdx - 1, 0);
    do {
      maxEndNodeIdx = maxEndNodeIdx + 1;
      range.setEnd(
        textNodes[Math.min(maxEndNodeIdx, textNodes.length)],
        textNodes[Math.min(maxEndNodeIdx, textNodes.length)].length
      );
    } while (
      maxEndNodeIdx < textNodes.length - 1 &&
      range.getBoundingClientRect().height < viewportHeight &&
      startNode.parentElement.closest(boundaryAncestorSelector) ===
        textNodes[maxEndNodeIdx].parentElement.closest(boundaryAncestorSelector)
    );
    maxEndNodeIdx = maxEndNodeIdx - 1;
    const maxRangeLength = range.toString().trim().length;

    // randomly select end node from the set of valid nodes, ensuring it
    // results in a selection at least of a certain size
    let endNodeIdx;
    let endNodeOffset;
    let tries = 100;
    const isRangeValid = (range: Range) =>
      !range.collapsed &&
      range.toString().trim().length > Math.min(10, maxRangeLength) &&
      range.getBoundingClientRect().height < viewportHeight &&
      range.getBoundingClientRect().height >= 8 &&
      range.getBoundingClientRect().width >= 8;

    const trimRangeStart = (range: Range): Range => {
      const getStartLeafTextNode = (node: Node): Node => {
        if (node.nodeType === Node.TEXT_NODE) {
          return node;
        }

        if (node.hasChildNodes() && node.firstChild) {
          return getStartLeafTextNode(node.firstChild);
        }

        // should never reach here
        return node;
      };
      const getSiblingNode = (node: Node) => {
        if (node.nextSibling) {
          return getStartLeafTextNode(node.nextSibling);
        }

        const getParentNextSibling = (node: Node): Node => {
          const parent = node.parentNode;
          const parentSibling = parent.nextSibling;
          if (parentSibling) {
            return parentSibling;
          }
          return getParentNextSibling(parent);
        };

        return getStartLeafTextNode(getParentNextSibling(node));
      };

      const node = range.startContainer;
      const rangeOfStartNode = new Range();
      rangeOfStartNode.setStart(node, range.startOffset);
      range.startContainer === range.endContainer
        ? rangeOfStartNode.setEnd(node, range.endOffset)
        : rangeOfStartNode.setEnd(node, node.textContent.length);
      const rangeOfStartNodeBB = rangeOfStartNode.getBoundingClientRect();

      // exclude this node from the range
      if (
        rangeOfStartNode.collapsed ||
        rangeOfStartNode.toString().trim().length === 0 ||
        rangeOfStartNodeBB.width === 0 ||
        rangeOfStartNodeBB.height === 0
      ) {
        const nextSibling = getSiblingNode(node);
        range.setStart(nextSibling, 0);

        return trimRangeStart(range);
      }

      return range;
    };

    const trimRangeEnd = (range: Range): Range => {
      const getEndLeafTextNode = (node: Node): Node => {
        if (node.nodeType === Node.TEXT_NODE) {
          return node;
        }

        if (node.hasChildNodes() && node.lastChild) {
          return getEndLeafTextNode(node.lastChild);
        }

        // should never reach here
        return node;
      };
      const getSiblingNode = (node: Node) => {
        if (node.previousSibling) {
          return getEndLeafTextNode(node.previousSibling);
        }

        const getParentPreviousSibling = (node: Node): Node => {
          const parent = node.parentNode;
          const parentSibling = parent.previousSibling;
          if (parentSibling) {
            return parentSibling;
          }
          return getParentPreviousSibling(parent);
        };

        return getEndLeafTextNode(getParentPreviousSibling(node));
      };

      const node = range.endContainer;
      const rangeOfEndNode = new Range();
      range.endContainer === range.startContainer
        ? rangeOfEndNode.setStart(node, range.startOffset)
        : rangeOfEndNode.setStart(node, 0);
      rangeOfEndNode.setEnd(node, range.endOffset);
      const rangeOfEndNodeBB = rangeOfEndNode.getBoundingClientRect();

      // exclude this node from the range
      if (
        rangeOfEndNode.collapsed ||
        rangeOfEndNode.toString().trim().length === 0 ||
        rangeOfEndNodeBB.width === 0 ||
        rangeOfEndNodeBB.height === 0
      ) {
        const prevSibling = getSiblingNode(node);
        range.setEnd(prevSibling, prevSibling.textContent.length);

        return trimRangeEnd(range);
      }

      return trimRangeEnd(trimRangeStart(range));
    };

    do {
      tries--;
      endNodeIdx =
        startNodeIdx +
        Math.round(Math.random() * (maxEndNodeIdx - startNodeIdx));
      endNodeOffset = Math.round(Math.random() * textNodes[endNodeIdx].length);

      range.setEnd(textNodes[endNodeIdx], endNodeOffset);
    } while (tries > 0 && !isRangeValid(range));

    return tries === 0 ? null : range;
  },
  scrollToRange: (range: Range, { width, height }: ViewportSize) => {
    window.getSelection().removeAllRanges();
    window.getSelection().addRange(range);

    const bb = range.getBoundingClientRect();
    const maxYOffset = height - bb.height;
    const maxXOffset = width - bb.width;

    // scroll the range into view, with some random offset
    document.scrollingElement.scrollTo({
      left:
        window.scrollX +
        range.getBoundingClientRect().left -
        maxXOffset * Math.random(),
      top:
        window.scrollY +
        range.getBoundingClientRect().top -
        maxYOffset * Math.random(),
      behavior: "auto",
    });
  },
  styleRange: (range: Range) => {
    const node = range.commonAncestorContainer;
    const elem =
      node.nodeType === Node.ELEMENT_NODE
        ? (node as HTMLElement)
        : node.parentElement;

    elem.style.lineHeight = "normal";
    elem.style.wordBreak = "break-all";
  },
  getSelectionAnnotations: (
    range: Range,
    { width: vWidth, height: vHeight }: ViewportSize
  ): SelectionAnnotation[] => {
    const bb = range.getBoundingClientRect();

    const startRange = new Range();
    startRange.setStart(range.startContainer, range.startOffset);
    startRange.setEnd(range.startContainer, range.startOffset);
    const startBB = startRange.getBoundingClientRect();

    const endRange = new Range();
    endRange.setStart(range.endContainer, range.endOffset);
    endRange.setEnd(range.endContainer, range.endOffset);
    const endBB = endRange.getBoundingClientRect();

    const annotations = [
      {
        label: "highlight",
        boundingBox: {
          xRelativeMin: bb.left / vWidth,
          yRelativeMin: bb.top / vHeight,
          xRelativeMax: bb.right / vWidth,
          yRelativeMax: bb.bottom / vHeight,
        },
      },
      startBB.left > bb.left &&
      (startBB.left > bb.left || startBB.bottom > bb.top)
        ? {
            label: "highlight_edge",
            boundingBox: {
              xRelativeMin: bb.left / vWidth,
              yRelativeMin: startBB.top / vHeight,
              xRelativeMax: startBB.left / vWidth,
              yRelativeMax: startBB.bottom / vHeight,
            },
          }
        : null,
      endBB.right < bb.right && endBB.top < bb.bottom
        ? {
            label: "highlight_edge",
            boundingBox: {
              xRelativeMin: endBB.right / vWidth,
              yRelativeMin: endBB.top / vHeight,
              xRelativeMax: bb.right / vWidth,
              yRelativeMax: endBB.bottom / vHeight,
            },
          }
        : null,
    ].filter(
      (h) =>
        Boolean(h) &&
        h.boundingBox.xRelativeMin !== h.boundingBox.xRelativeMax &&
        h.boundingBox.yRelativeMin !== h.boundingBox.yRelativeMax
    );

    return annotations as SelectionAnnotation[];
  },
};
