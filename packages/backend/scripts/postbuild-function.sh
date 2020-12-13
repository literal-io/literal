#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`realpath "$(dirname $OWN_PATH)/../amplify/backend/function/"`

GRAPHQL_RESOLVER_SRC_DIR="$FUNCTION_DIR/GraphQLResolver"
CLOUDFRONT_ORIGIN_REQUEST_SRC_DIR="$FUNCTION_DIR/CloudfrontOriginRequest"

# Prune build-time dependencies and install any runtime dependencies
(cd $GRAPHQL_RESOLVER_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)

(cd $CLOUDFRONT_ORIGIN_REQUEST_SRC_DIR && \
  rm -rf node_modules && \
  cd ./src && \
  npm install --production)
