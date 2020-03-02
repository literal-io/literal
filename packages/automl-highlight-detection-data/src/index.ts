import {firefox, BrowserContext} from 'playwright'
import {v4 as uuid} from 'uuid'
import {resolve} from 'path'
import {getScreenshot, getBrowserContext} from './lib/playwright'
import {browserInject, DOMAIN} from './browser-inject'

const OUTPUT_DIR = resolve(__dirname, '../output/screenshot')

const exec = async ({ context }: { context: BrowserContext }) => {
  const outputPath = resolve(OUTPUT_DIR, `${uuid()}_firefox_wikipedia_pixel-2.png`)

  await getScreenshot({
    context,
    href: 'https://en.wikipedia.org/wiki/Baroque',
    outputPath,
    inject: browserInject(DOMAIN.WIKIPEDIA)
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
