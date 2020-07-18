#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`realpath "$(dirname $OWN_PATH)/../amplify/backend/function/"`

GRAPHQL_RESOLVER_SRC_DIR="$FUNCTION_DIR/GraphQLResolver"
CLOUDFRONT_ORIGIN_REQUEST_SRC_DIR="$FUNCTION_DIR/CloudfrontOriginRequest"
POST_AUTHENTICATION_SRC_DIR="$FUNCTION_DIR/PostAuthentication"
DYNAMODB_STREAM_SRC_DIR="$FUNCTION_DIR/DynamoDBStream"
SYNC_ANNOTATION_COLLECTION_SRC_DIR="$FUNCTION_DIR/SyncAnnotationCollection"

# Prune build-time dependencies and install any runtime dependencies
(cd $GRAPHQL_RESOLVER_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)

(cd $CLOUDFRONT_ORIGIN_REQUEST_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)

(cd $POST_AUTHENTICATION_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)

(cd $DYNAMODB_STREAM_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)

(cd $SYNC_ANNOTATION_COLLECTION_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)
