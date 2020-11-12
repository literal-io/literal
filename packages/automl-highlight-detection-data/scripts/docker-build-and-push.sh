#!/bin/bash

set -ex

docker-compose -f docker-compose.dev.yml build
docker-compose -f docker-compose.dev.yml push
