import { resolve } from "path";
import { writeFileSync, mkdirSync } from "fs";

import { v4 as uuid } from "uuid";
import * as R from "ramda";
import { Storage } from "@google-cloud/storage";
import pSeries from "p-series";
import pRetry from "p-retry";

import { DOMAIN, parsers, SelectionAnnotation } from "./browser-inject";
import {
  DEBUG,
  OUTPUT_DIR,
  OUTPUT_SIZE,
  GCLOUD_PROJECT_ID,
  GCLOUD_SERVICE_ACCOUNT_FILENAME,
  GCLOUD_BUCKET_NAME,
} from "./constants";
import { AppiumDriver, DriverInterface } from "./drivers";

const storage = new Storage({
  projectId: GCLOUD_PROJECT_ID,
  keyFilename: GCLOUD_SERVICE_ACCOUNT_FILENAME,
});

type Task = {
  jobId: string;
  driver: DriverInterface;
  domain: DOMAIN;
  href: string;
};

const execTask = async (task: Task) => {
  const fileName = `${uuid()}_chrome_pixel-2.png`;
  const outputPath = resolve(OUTPUT_DIR, fileName);
  const annotations = await task.driver.getScreenshot({
    domain: task.domain,
    outputPath,
    href: task.href,
    forceNavigate: false,
  });

  await storage
    .bucket(GCLOUD_BUCKET_NAME)
    .upload(outputPath, { destination: `${task.jobId}/${fileName}` });

  return annotations.map((a) => ({
    url: `gs://${GCLOUD_BUCKET_NAME}/${task.jobId}/${fileName}`,
    ...a,
  }));
};

const processResults = async (
  jobId: string,
  results: Array<SelectionAnnotation & { url: string }>
) => {
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

  await storage
    .bucket(GCLOUD_BUCKET_NAME)
    .upload(manifestPath, { destination: `${jobId}/manifest.csv` });

  console.log(`Upload complete. Job ID: ${jobId}`);
};

(async () => {
  const driver = new AppiumDriver();
  await driver.initializeContext({
    browser: "Chrome",
    device: "Android Emulator",
  });
  const jobId = uuid();

  mkdirSync(OUTPUT_DIR, { recursive: true })

  const tasks = R.sortBy(
    R.prop("href"),
    R.times(() => {
      const domain = Object.values(DOMAIN)[
        Math.floor(Math.random() * Object.values(DOMAIN).length)
      ];
      const href = parsers[domain].getUrl();
      return {
        jobId,
        driver,
        domain,
        href,
      };
    }, OUTPUT_SIZE)
  );
  const taskThunks = tasks.map((task: Task) => () =>
    pRetry(
      () =>
        execTask(task).then((res) => {
          if (res.length === 0) {
            // if we didn't return annotations for any reason, throw
            // an error so that the task is retried.
            throw new Error();
          }
          return res;
        }),
      {
        minTimeout: 1,
        maxTimeout: Infinity,
        factor: 1,
        retries: 10,
        onFailedAttempt: (error) => {
          console.error('failed attempt', error)
        }
      }
    ).catch((err) => {
      console.error(err)
      return []
    })
  );

  const results = await pSeries(taskThunks).then(R.flatten);

  await processResults(jobId, results);

  await driver.cleanup();
})();
