#!/bin/bash

STACK_NAME=automl-highlight-detection-data
SERVICE_DOCKER_IMAGE_URL=""

aws cloudformation update-stack \
  --stack-name $STACK_NAME 
  --template-url s3://automl-highlight-detection-data-deployment/main.json
  --parameters \
    ParameterKey=ServiceDokerImageUrl,ParameterValue=$SERVICE_DOCKER_IMAGE_URL
