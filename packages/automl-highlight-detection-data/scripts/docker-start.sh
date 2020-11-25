#!/bin/bash

set -ex

docker-compose -f docker-compose.dev.yml up --build &> local-docker.log
