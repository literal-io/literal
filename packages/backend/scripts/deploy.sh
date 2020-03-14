#!/bin/bash

# preconditions:
#
# - `lerna bootstrap` executed at repo root

set -ex

OWN_PATH=`realpath $0`
SCRIPT_DIR=`dirname $OWN_PATH`

# build function
("$SCRIPT_DIR/prebuild-function.sh")
("$SCRIPT_DIR/build-function.sh")
("$SCRIPT_DIR/postbuild-function.sh")

amplify push -y
