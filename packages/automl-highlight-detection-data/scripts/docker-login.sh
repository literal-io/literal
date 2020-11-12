#!/bin/bash

set -ex

gcloud auth activate-service-account  --key-file=gcloud-service-account.json
yes | gcloud auth configure-docker
