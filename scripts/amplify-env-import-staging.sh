#!/bin/bash

# Used for scaffolding a new environment based on staging. 

set -e
IFS='|'
OWN_PATH=`realpath $0`
ROOT_DIR=`readlink -f "$(dirname $OWN_PATH)/../"`
AWSCLOUDFORMATIONCONFIG="{\
  \"Region\": \"us-east-1\",\
  \"DeploymentBucketName\": \"amplify-literal-staging-211408-deployment\",\
  \"UnauthRoleName\": \"amplify-literal-staging-211408-unauthRole\",\
  \"StackName\": \"amplify-literal-staging-211408\",\
  \"StackId\": \"arn:aws:cloudformation:us-east-1:046525304497:stack/amplify-literal-staging-211408/c067b540-005e-11eb-aa67-0e80bfdca6c9\",\
  \"AuthRoleName\": \"amplify-literal-staging-211408-authRole\",\
  \"UnauthRoleArn\": \"arn:aws:iam::046525304497:role/amplify-literal-staging-211408-unauthRole\",\
  \"AuthRoleArn\": \"arn:aws:iam::046525304497:role/amplify-literal-staging-211408-authRole\"\
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
  --name staging \
  --config $PROVIDER_CONFIG \
  --awsInfo $AWS_CONFIG \
  --yes
