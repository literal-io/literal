#!/bin/bash
set -e

# The absolute path of the file that was built by bsb.
FILE_PATH="$1"
PAGE_REGEX="^(.*)__(\w+)__\.js$"

if [[ $FILE_PATH =~ $PAGE_REGEX ]]; then
  DYN_ARG="${BASH_REMATCH[2]}"
  DIR=$(dirname "${FILE_PATH}")
  mv $FILE_PATH "${DIR}/[${DYN_ARG}].js"
fi
