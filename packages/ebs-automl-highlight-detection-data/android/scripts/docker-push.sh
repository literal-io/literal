#!/bin/bash

set -ex

IMAGE_NAME=automl-highlight-detection-data-android
GCP_HOSTNAME=us.gcr.io
GCP_PROJECT_ID=literal-269716

REPOSITORY_TAG="${GCP_HOSTNAME}/${GCP_PROJECT_ID}/${IMAGE_NAME}"

docker push $REPOSITORY_TAG
