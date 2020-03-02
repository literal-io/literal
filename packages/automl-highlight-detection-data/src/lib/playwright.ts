import {devices, Browser, BrowserContext} from 'playwright'
import {Injectable} from '../browser-inject'

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
  inject
}: {context: BrowserContext, href: string, outputPath: string, inject: Injectable}) => {
  const page = await context.newPage()
  await page.goto(href)
  await page.evaluate(inject.handler, inject.args)
  await page.screenshot({path: outputPath})
  await page.close()
}
