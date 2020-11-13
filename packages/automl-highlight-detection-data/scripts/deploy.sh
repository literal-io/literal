#!/bin/bash

set -ex

./scripts/docker-login.sh
./scripts/docker-build-and-push.sh
./scripts/gcloud-create-instance.sh
