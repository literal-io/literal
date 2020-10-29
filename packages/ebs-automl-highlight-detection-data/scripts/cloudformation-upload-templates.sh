#!/bin/sh

aws s3 sync ./cloudformation s3://automl-highlight-detection-data-deployment
