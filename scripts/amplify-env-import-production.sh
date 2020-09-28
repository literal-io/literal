#!/bin/bash

# Used for scaffolding a new environment based on production. 

set -e
IFS='|'
OWN_PATH=`realpath $0`
ROOT_DIR=`readlink -f "$(dirname $OWN_PATH)/../"`
AWSCLOUDFORMATIONCONFIG="{\
  \"Region\": \"us-east-1\",\
  \"DeploymentBucketName\": \"amplify-literal-production-114446-deployment\",\
  \"UnauthRoleName\": \"amplify-literal-production-114446-unauthRole\",\
  \"StackName\": \"amplify-literal-production-114446\",\
  \"StackId\": \"arn:aws:cloudformation:us-east-1:046525304497:stack/amplify-literal-production-114446/cc4d0500-5b12-11ea-883c-12002b7e66d1\",\
  \"AuthRoleName\": \"amplify-literal-production-114446-authRole\",\
  \"UnauthRoleArn\": \"arn:aws:iam::046525304497:role/amplify-literal-production-114446-unauthRole\",\
  \"AuthRoleArn\": \"arn:aws:iam::046525304497:role/amplify-literal-production-114446-authRole\"\
}"
PROVIDER_CONFIG="{\
  \"awscloudformation\":$AWSCLOUDFORMATIONCONFIG\
}"
AWS_CONFIG="{\
  \"configLevel\":\"project\",\
  \"useProfile\":true,\
  \"profileName\":\"amplify-literal\"\
}"

amplify env import \
  --name production \
  --config $PROVIDER_CONFIG \
  --awsInfo $AWS_CONFIG \
  --yes
