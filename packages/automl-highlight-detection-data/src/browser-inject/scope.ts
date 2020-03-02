import {InjectScope} from './types'
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
    let maxEndNodeIdx = startNodeIdx
    while (
      range.setEnd(textNodes[maxEndNodeIdx], 0),
      range.getBoundingClientRect().height < viewportHeight
    ) {maxEndNodeIdx++}
    maxEndNodeIdx = maxEndNodeIdx - 1

    // randomly select end node from the set of valid nodes
    const endNodeIdx = startNodeIdx + Math.round(Math.random() * (maxEndNodeIdx - startNodeIdx))
    // randomly select an offset within the end node
    // TODO: weight begin / end of the node more than the middle?
    const endNodeOffset = Math.round(Math.random() * (textNodes[endNodeIdx].length - 1))

    range.setEnd(textNodes[endNodeIdx], endNodeOffset)

    return range
  },
  scrollToRange: (range: Range) => {
    document.getSelection().removeAllRanges()
    document.getSelection().addRange(range)

    // scroll the range into view, with some random offset
    window.scroll(0, range.getBoundingClientRect().top)
  }
}

