import {InjectScope, SelectionAnnotation} from './types'

export const scope: InjectScope = {
  getTextNodes: (el: HTMLElement): Text[] => {
    const walker = document.createTreeWalker(el, NodeFilter.SHOW_TEXT);
    const nodes = [];
    while (walker.nextNode()) {
      nodes.push(walker.currentNode as Text);
    }
    return nodes;
  },
  getRandomRange: (textNodes: Text[]): Range => {
    const viewportHeight = document.documentElement.clientHeight
    const startNodeIdx = Math.round(Math.random() * (textNodes.length - 1))
    const startNode = textNodes[startNodeIdx]

    const range = document.createRange()
    range.setStart(startNode, 0)

    // walk ranges of increasing distance until we exceed the viewport height
    let maxEndNodeIdx = Math.max(startNodeIdx - 1, 0)
    do {
      maxEndNodeIdx = maxEndNodeIdx + 1
      range.setEnd(textNodes[maxEndNodeIdx], textNodes[maxEndNodeIdx].length)
    } while (
      maxEndNodeIdx < textNodes.length - 1 &&
      range.getBoundingClientRect().height < viewportHeight
    )
    maxEndNodeIdx = maxEndNodeIdx - 1
    const maxRangeLength = range.toString().trim().length

    // randomly select end node from the set of valid nodes, ensuring it results in a selection
    // at least of a certain size
    let endNodeIdx
    let endNodeOffset
    do {
      endNodeIdx = startNodeIdx + Math.round(Math.random() * (maxEndNodeIdx - startNodeIdx))
      endNodeOffset = Math.round(Math.random() * (textNodes[endNodeIdx].length))

      range.setEnd(textNodes[endNodeIdx], endNodeOffset)
    } while (
      range.collapsed ||
      range.toString().trim().length < Math.min(10, maxRangeLength) ||
      range.getBoundingClientRect().height > viewportHeight
    )

    return range
  },
  scrollToRange: (range: Range) => {
    window.getSelection().removeAllRanges()
    window.getSelection().addRange(range)

    const bb = range.getBoundingClientRect()
    const maxYOffset = document.documentElement.clientHeight - bb.height
    const maxXOffset = document.documentElement.clientWidth - bb.width

    // scroll the range into view, with some random offset
    window.scroll(
      window.scrollX + range.getBoundingClientRect().left - (maxXOffset * Math.random()),
      window.scrollY + range.getBoundingClientRect().top - (maxYOffset * Math.random())
    )
  },
  getSelectionAnnotations: (range: Range): SelectionAnnotation[] => {
    const vWidth = document.documentElement.clientWidth
    const vHeight = document.documentElement.clientHeight
    const bb = range.getBoundingClientRect()

    const startRange = new Range()
    startRange.setStart(range.startContainer, range.startOffset)
    startRange.setEnd(range.startContainer, (range.startContainer as Text).length)
    const startBB = startRange.getBoundingClientRect()

    const endRange = new Range()
    endRange.setStart(range.endContainer, 0)
    endRange.setEnd(range.endContainer, range.endOffset)
    const endBB = endRange.getBoundingClientRect()

    const annotations = [
      {
        label: 'highlight',
        boundingBox: {
          xRelativeMin: bb.left / vWidth,
          yRelativeMin: bb.top / vHeight,
          xRelativeMax: bb.right / vWidth,
          yRelativeMax: bb.bottom / vHeight
        }
      },
      startBB.left > bb.left
        ? {
          label: 'highlight_edge',
          boundingBox: {
            xRelativeMin: bb.left / vWidth,
            yRelativeMin: bb.top / vHeight,
            xRelativeMax: startBB.left / vWidth,
            yRelativeMax: startBB.bottom / vHeight
          }
        }
        : null,
      endBB.right < bb.right
        ? {
          label: 'highlight_edge',
          boundingBox: {
            xRelativeMin: endBB.right / vWidth,
            yRelativeMin: endBB.top / vHeight,
            xRelativeMax: bb.right / vWidth,
            yRelativeMax: bb.bottom / vHeight
          }
        }
        : null
    ].filter(Boolean)

    return annotations as SelectionAnnotation[]
  }
}

