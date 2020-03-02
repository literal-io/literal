import {firefox, BrowserContext} from 'playwright'
import {v4 as uuid} from 'uuid'
import * as R from 'ramda'
import {Storage} from '@google-cloud/storage'
import {getScreenshot, getBrowserContext} from './lib/playwright'
import {DOMAIN} from './browser-inject'

import {resolve} from 'path'
import {writeFileSync} from 'fs'

const OUTPUT_DIR = resolve(__dirname, '../output/screenshot')
const GCLOUD_SERVICE_ACCOUNT_FILENAME = resolve(__dirname, '../gcloud-service-account.json')
const GCLOUD_PROJECT_ID = 'literal-269716'
const GCLOUD_BUCKET_NAME = 'literal-screenshot'

const exec = async ({context}: {context: BrowserContext}) => {
  const fileName = `${uuid()}_firefox_wikipedia_pixel-2.png`
  const outputPath = resolve(OUTPUT_DIR, fileName)
  const annotations = await getScreenshot({
    context,
    href: 'https://en.wikipedia.org/wiki/Baroque',
    outputPath,
    domain: DOMAIN.WIKIPEDIA
  })

  return {screenshotPath: outputPath, annotations, screenshotFileName: fileName}
}

(async () => {
  const browser = await firefox.launch()
  const context = await getBrowserContext({browser, device: 'Pixel 2'})
  const storage = new Storage({
    projectId: GCLOUD_PROJECT_ID,
    keyFilename: GCLOUD_SERVICE_ACCOUNT_FILENAME
  })
  const jobId = uuid()

  const results = await Promise.all(R.times(async () => {
    const {screenshotPath, annotations, screenshotFileName: destinationName} = await exec({context})
    await storage
      .bucket(GCLOUD_BUCKET_NAME)
      .upload(screenshotPath, {destination: `${jobId}/${destinationName}`})

    return annotations.map(a => ({
      url: `gs://${GCLOUD_BUCKET_NAME}/${jobId}/${destinationName}`,
      ...a
    }))
  }, 5)).then(R.flatten)


  const csv = results.map(({
    url,
    label,
    boundingBox: {xRelativeMin, yRelativeMin, xRelativeMax, yRelativeMax}
  }) => {
    return [
      'UNASSIGNED',
      url,
      label,
      xRelativeMin,
      yRelativeMin,
      ,
      ,
      xRelativeMax,
      yRelativeMax,
      ,
      ,
    ]
  }).map(row => row.join(',')).join('\n')
  const manifestPath = resolve(OUTPUT_DIR, 'manifest.csv')
  writeFileSync(manifestPath, csv)

  await storage.bucket(GCLOUD_BUCKET_NAME).upload(
    manifestPath,
    {destination: `${jobId}/manifest.csv`}
  )

  await browser.close()
})()
