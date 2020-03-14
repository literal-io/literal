#!/bin/bash

set -ex

OWN_PATH=`realpath $0`
FUNCTION_DIR=`realpath "$(dirname $OWN_PATH)/../amplify/backend/function/"`

GRAPHQL_RESOLVER_SRC_DIR="$FUNCTION_DIR/GraphQLResolver/src"

# Install depedencies for each of the functions.
(cd $GRAPHQL_RESOLVER_SRC_DIR && npm install)

# Sync against the currently deployed GraphQL schema.
(cd $GRAPHQL_RESOLVER_SRC_DIR && \
  npm run graphql:sync-schema && \
  npm link @trashed/bs-aws-amplify)