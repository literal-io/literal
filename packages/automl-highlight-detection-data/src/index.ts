import { resolve } from "path";
import { writeFileSync } from "fs";

import { v4 as uuid } from "uuid";
import * as R from "ramda";
import { Storage } from "@google-cloud/storage";
import limit from "p-limit";

import { DOMAIN } from "./browser-inject";

import { PlaywrightDriver, AppiumDriver, DriverInterface } from "./drivers";

const DEBUG = true;
const DRIVER = "appium"; // 'playwright'
const OUTPUT_DIR = resolve(__dirname, "../output/screenshot");
const GCLOUD_SERVICE_ACCOUNT_FILENAME = resolve(
  __dirname,
  "../gcloud-service-account.json"
);
const GCLOUD_PROJECT_ID = "literal-269716";
const GCLOUD_BUCKET_NAME = "literal-screenshot";
const OUTPUT_SIZE = 5;

const pLimit = limit(1);

const exec = async (driver: DriverInterface) => {
  const fileName = `${uuid()}_firefox_wikipedia_pixel-2.png`;
  const outputPath = resolve(OUTPUT_DIR, fileName);
  const annotations = await driver.getScreenshot({
    domain: DOMAIN.WIKIPEDIA,
    outputPath,
  });

  return {
    screenshotPath: outputPath,
    annotations,
    screenshotFileName: fileName,
  };
};

(async () => {
  const driver =
    DRIVER === "appium" ? new AppiumDriver() : new PlaywrightDriver();
  await driver.initializeContext(
    DRIVER === "appium"
      ? {
          browser: "Chrome",
          device: "Android Emulator",
        }
      : {
          browser: "firefox",
          device: "Pixel 2",
        }
  );
  const storage = new Storage({
    projectId: GCLOUD_PROJECT_ID,
    keyFilename: GCLOUD_SERVICE_ACCOUNT_FILENAME,
  });
  const jobId = uuid();

  const run = async () => {
    const {
      screenshotPath,
      annotations,
      screenshotFileName: destinationName,
    } = await exec(driver);
    await storage
      .bucket(GCLOUD_BUCKET_NAME)
      .upload(screenshotPath, { destination: `${jobId}/${destinationName}` });

    return annotations.map((a) => ({
      url: `gs://${GCLOUD_BUCKET_NAME}/${jobId}/${destinationName}`,
      ...a,
    }));
  };

  const results = await Promise.all(
    R.times(
      () =>
        pLimit(() =>
          run().catch((err) => {
            return [];
          })
        ),
      OUTPUT_SIZE
    )
  ).then(R.flatten);

  const csv = results
    .map(
      ({
        url,
        label,
        boundingBox: { xRelativeMin, yRelativeMin, xRelativeMax, yRelativeMax },
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
    .map((row) => row.join(","))
    .join("\n");
  const manifestPath = resolve(OUTPUT_DIR, "manifest.csv");
  writeFileSync(manifestPath, csv);

  if (!DEBUG) {
    await storage
      .bucket(GCLOUD_BUCKET_NAME)
      .upload(manifestPath, { destination: `${jobId}/manifest.csv` });

    console.log(`Upload complete. Job ID: ${jobId}`);
  }

  await driver.cleanup();
})();
