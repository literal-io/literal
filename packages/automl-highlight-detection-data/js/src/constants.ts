import { resolve } from "path";

export const DEBUG = process.env.NODE_ENV !== "production";
export const OUTPUT_SIZE = parseInt(process.env.OUTPUT_SIZE || "20");

console.log("DEBUG: ", DEBUG)
console.log("OUTPUT_SIZE: ", OUTPUT_SIZE)

export const OUTPUT_DIR = resolve(__dirname, "../output/screenshot");
export const GCLOUD_SERVICE_ACCOUNT_FILENAME = resolve(
  __dirname,
  "../gcloud-service-account.json"
);
export const GCLOUD_PROJECT_ID = "literal-269716";
export const GCLOUD_BUCKET_NAME = "literal-screenshot";
