#!/bin/bash

GCS_BUCKET_NAME="gs://literal-screenshot"

npm run build

gcloud functions deploy automlManifestMerger \
  --runtime nodejs12 \
  --trigger-resource $GCS_BUCKET_NAME \
  --trigger-event google.storage.object.finalize \
  --env-vars-file .env.yaml
