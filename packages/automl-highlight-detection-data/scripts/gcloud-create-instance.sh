#!/bin/bash

set -ex

# gcp config
GCP_REGION=us-east-4
GCP_ZONE=us-east4-c
GCP_PROJECT_ID=literal-269716

# disk config
DISK_NAME=debian-10-nested-virtualization
CREATE_DISK=false

# instance config
INSTANCE_ID=1
MACHINE_TYPE=n1-standard-8

# image config
IMAGE_NAME=automl-highlight-detection-data
GCP_HOSTNAME=us.gcr.io
CONTAINER_IMAGE_URL="${GCP_HOSTNAME}/${GCP_PROJECT_ID}/${IMAGE_NAME}:latest"

if $CREATE_DISK; then
  gcloud compute disks create "$DISK_NAME" \
    --image-project debian-cloud \
    --image-family debian-10 \
    --size 50GB \
    --zone "$GCP_ZONE"
fi

if $CREATE_DISK; then
  gcloud compute images create "$DISK_NAME" \
    --source-disk "$DISK_NAME" \
    --source-disk-zone "$GCP_ZONE" \
    --licenses "https://www.googleapis.com/compute/v1/projects/vm-options/global/licenses/enable-vmx"
fi

gcloud compute instances create "automl-highlight-detection-data-${INSTANCE_ID}" \
  --zone "$GCP_ZONE" \
  --machine-type "$MACHINE_TYPE" \
  --min-cpu-platform "Intel Haswell" \
  --image debian-10-nested-virtualization \
  --metadata-from-file startup-script=./scripts/gcloud-instance-startup-script.sh
