import {firefox, devices, BrowserContext, Browser} from 'playwright'
import {v4 as uuid} from 'uuid'
import {resolve} from 'path'
import * as R from 'ramda'

const OUTPUT_DIR = resolve(__dirname, '../output/screenshot')

const getBrowserContext = ({browser, device = 'Pixel 2'}: {browser: Browser, device: string}): Promise<BrowserContext> => {
  const d = devices[device]

  return browser.newContext({
    viewport: d.viewport,
    userAgent: d.userAgent
  })
}

const getScreenshot = async ({
  context,
  href,
  outputPath,
  onReady
}: {context: BrowserContext, href: string, outputPath: string, onReady: () => void}) => {
  const page = await context.newPage()
  await page.goto(href)
  await page.evaluate(onReady)
  await page.screenshot({path: outputPath})
  await page.close()
}

const exec = async ({ context }: { context: BrowserContext }) => {
  const outputPath = resolve(OUTPUT_DIR, `${uuid()}_firefox_wikipedia_pixel-2.png`)

  await getScreenshot({
    context,
    href: 'https://en.wikipedia.org/wiki/Baroque',
    outputPath,
    onReady: () => {

      const getTextNodes = (el: HTMLElement): Text[] => {
        const walker = document.createTreeWalker(el, NodeFilter.SHOW_TEXT);
        const nodes = [];
        while (walker.nextNode()) {
          nodes.push(walker.currentNode as Text);
        }
        return nodes;
      }

      const getRandomRange = (textNodes: Text[]): Range => {
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
      }

      // open all closed sections
      document
        .querySelectorAll('#bodyContent .collapsible-heading,.collapsible-block')
        .forEach((elem) => {elem.classList.add("open-block")})

      const textNodes = getTextNodes(document.querySelector('#bodyContent'))

      // get and select a random range
      const range = getRandomRange(textNodes)
      document.getSelection().removeAllRanges()
      document.getSelection().addRange(range)

      // scroll the range into view, with some random offset
      window.scroll(0, range.getBoundingClientRect().top)
    }
  })
}

(async () => {
  const browser = await firefox.launch()
  const context = await getBrowserContext({browser, device: 'Pixel 2'})

  await exec({ context })
  await exec({ context })
  await exec({ context })
  await exec({ context })
  await exec({ context })
  
  await browser.close()
})()
