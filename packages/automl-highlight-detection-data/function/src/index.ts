import { CloudFunctionsContext } from "@google-cloud/functions-framework/build/src/functions";
import { Storage } from "@google-cloud/storage";

const storage = new Storage();

type StorageFunction = (
  data: {
    bucket: string;
    name: string;
  },
  context: CloudFunctionsContext
) => Promise<any>;

const getFileData = (bucket: string, path: string) =>
  storage
    .bucket(bucket)
    .file(path)
    .download()
    .then(([data]) => data.toString());

export const automlManifestMerger: StorageFunction = async (data, context) => {
  console.log(JSON.stringify(data));
  if (!data.name.endsWith("manifest.csv")) {
    /** no work to do, skipping **/
    return Promise.resolve();
  }

  const targetManifest = storage
    .bucket(process.env.GCS_TARGET_BUCKET)
    .file(process.env.GCS_TARGET_MANIFEST);

  return targetManifest
    .exists()
    .then(([exists]) => {
      if (exists) {
        return getFileData(
          process.env.GCS_TARGET_BUCKET,
          process.env.GCS_TARGET_MANIFEST
        );
      }
      return "";
    })
    .then((targetData) =>
      Promise.all([
        Promise.resolve(targetData),
        getFileData(data.bucket, data.name),
      ])
    )
    .then((resultData) => {
      const mergedData = resultData.join("\n");
      return targetManifest.save(mergedData, { resumable: false });
    });
};
