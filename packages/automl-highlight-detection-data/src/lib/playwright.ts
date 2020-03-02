import {devices, Browser, BrowserContext} from 'playwright'
import { SelectionAnnotation, browserInject, DOMAIN} from '../browser-inject'

export const getBrowserContext = ({browser, device = 'Pixel 2'}: {browser: Browser, device: string}): Promise<BrowserContext> => {
  const d = devices[device]

  return browser.newContext({
    viewport: d.viewport,
    userAgent: d.userAgent
  })
}

export const getScreenshot = async ({
  context,
  href,
  outputPath,
  domain
}: {context: BrowserContext, href: string, outputPath: string, domain: DOMAIN}): Promise<SelectionAnnotation[]> => {
  const page = await context.newPage()
  await page.goto(href)

  const annotations = await browserInject(domain, page)

  await page.screenshot({path: outputPath})
  await page.close()

  return annotations
}
