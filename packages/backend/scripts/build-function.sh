#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`realpath "$(dirname $OWN_PATH)/../amplify/backend/function/"`


GRAPHQL_RESOLVER_SRC_DIR="$FUNCTION_DIR/GraphQLResolver"
CLOUDFRONT_ORIGIN_REQUEST_SRC_DIR="$FUNCTION_DIR/CloudfrontOriginRequest"

# Build each of the functions.
(cd $GRAPHQL_RESOLVER_SRC_DIR && npm run build)
(cd $CLOUDFRONT_ORIGIN_REQUEST_SRC_DIR && npm run build)
