#!/bin/bash

set -ex

STACK_NAME=automl-highlight-detection-data
SERVICE_DOCKER_IMAGE_URL="046525304497.dkr.ecr.us-east-1.amazonaws.com/automl-highlight-detection-data"

aws cloudformation create-stack \
  --stack-name $STACK_NAME \
  --template-url https://automl-highlight-detection-data-deployment.s3.amazonaws.com/main.json \
  --parameters \
    ParameterKey=ServiceDockerImageUrl,ParameterValue="$SERVICE_DOCKER_IMAGE_URL"
