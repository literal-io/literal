import { v4 as uuid } from "uuid";
import * as R from "ramda";
import { Storage } from "@google-cloud/storage";
import { DOMAIN } from "./browser-inject";
import limit from "p-limit";

import { PlaywrightDriver, DriverInterface } from "./drivers";

import { resolve } from "path";
import { writeFileSync } from "fs";

const OUTPUT_DIR = resolve(__dirname, "../output/screenshot");
const GCLOUD_SERVICE_ACCOUNT_FILENAME = resolve(
  __dirname,
  "../gcloud-service-account.json"
);
const GCLOUD_PROJECT_ID = "literal-269716";
const GCLOUD_BUCKET_NAME = "literal-screenshot";
const OUTPUT_SIZE = 500;

const pLimit = limit(25);

const exec = async (driver: DriverInterface) => {
  const fileName = `${uuid()}_firefox_wikipedia_pixel-2.png`;
  const outputPath = resolve(OUTPUT_DIR, fileName);
  const annotations = await driver.getScreenshot({
    href: "https://en.wikipedia.org/wiki/Special:Random",
    outputPath,
    domain: DOMAIN.WIKIPEDIA
  });

  return {
    screenshotPath: outputPath,
    annotations,
    screenshotFileName: fileName
  };
};

(async () => {
  const driver = new PlaywrightDriver();
  await driver.initializeContext({
    browser: "firefox",
    device: "Pixel 2"
  });
  const storage = new Storage({
    projectId: GCLOUD_PROJECT_ID,
    keyFilename: GCLOUD_SERVICE_ACCOUNT_FILENAME
  });
  const jobId = uuid();

  const run = async () => {
    const {
      screenshotPath,
      annotations,
      screenshotFileName: destinationName
    } = await exec(driver);
    await storage
      .bucket(GCLOUD_BUCKET_NAME)
      .upload(screenshotPath, { destination: `${jobId}/${destinationName}` });

    return annotations.map(a => ({
      url: `gs://${GCLOUD_BUCKET_NAME}/${jobId}/${destinationName}`,
      ...a
    }));
  };

  const results = await Promise.all(
    R.times(() => pLimit(() => run()), OUTPUT_SIZE)
  ).then(R.flatten);

  const csv = results
    .map(
      ({
        url,
        label,
        boundingBox: { xRelativeMin, yRelativeMin, xRelativeMax, yRelativeMax }
      }) => {
        return [
          "UNASSIGNED",
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
        ];
      }
    )
    .map(row => row.join(","))
    .join("\n");
  const manifestPath = resolve(OUTPUT_DIR, "manifest.csv");
  writeFileSync(manifestPath, csv);

  await storage
    .bucket(GCLOUD_BUCKET_NAME)
    .upload(manifestPath, { destination: `${jobId}/manifest.csv` });

  await driver.cleanup();

  console.log(`Upload complete. Job ID: ${jobId}`);
})();
