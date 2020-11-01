#!/bin/bash

set -ex

./scripts/docker-login.sh
./scripts/docker-build.sh
./scripts/docker-push.sh
./scripts/cloudformation-upload-templates.sh
./scripts/cloudformation-update-stack.sh
