#!/bin/bash

set -ex

AWS_REGION=us-east-1
AWS_ACCOUNT_ID=046525304497

aws ecr get-login-password --region $AWS_REGION | docker login --username AWS --password-stdin "${AWS_ACCOUNT_ID}.dkr.ecr.${AWS_REGION}.amazonaws.com"
