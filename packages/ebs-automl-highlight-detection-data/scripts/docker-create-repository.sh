#!/bin/bash

AWS_REGION=us-east-1
REPOSITORY_NAME=automl-highlight-detection-data

aws ecr create-repository \
    --repository-name $REPOSITORY_NAME \
    --image-scanning-configuration scanOnPush=true \
    --region $AWS_REGION 
