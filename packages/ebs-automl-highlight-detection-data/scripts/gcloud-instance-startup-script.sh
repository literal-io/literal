#!/bin/bash

set -ex

IMAGE_URL=us.gcr.io/literal-269716/automl-highlight-detection-data:latest

sudo apt-get update

# install monitoring agent
curl -sSO https://dl.google.com/cloudagents/add-monitoring-agent-repo.sh
sudo bash add-monitoring-agent-repo.sh
sudo apt-get update
sudo apt-get install -y stackdriver-agent
sudo service stackdriver-agent start

# install logging agent
curl -sSO https://dl.google.com/cloudagents/add-logging-agent-repo.sh
sudo bash add-logging-agent-repo.sh
sudo apt-get update
sudo apt-get install -y google-fluentd
sudo apt-get install -y google-fluentd-catch-all-config-structured
sudo service google-fluentd start

# install docker
sudo apt-get install -y \
  apt-transport-https \
  ca-certificates \
  curl \
  gnupg-agent \
  software-properties-common
curl -fsSL https://download.docker.com/linux/debian/gpg | sudo apt-key add -
sudo add-apt-repository \
   "deb [arch=amd64] https://download.docker.com/linux/debian \
   $(lsb_release -cs) \
   stable"
sudo apt-get update
sudo apt-get install -y docker-ce docker-ce-cli containerd.io
sudo groupadd docker
sudo usermod -aG docker $USER
newgrp docker
yes | gcloud auth configure-docker

# start the container
docker run -d --privileged --log-driver=gcplogs "$IMAGE_URL" 
