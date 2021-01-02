#!/bin/bash
set -e

OWN_PATH=`realpath $0`
ROOT_DIR=`realpath "$(dirname $OWN_PATH)/../"`
AWS_PROFILE=amplify-literal

aws appsync get-introspection-schema \
  --format JSON \
  --api-id $API_ID \
  --profile $AWS_PROFILE \
  "$ROOT_DIR/graphql-schema.json"

