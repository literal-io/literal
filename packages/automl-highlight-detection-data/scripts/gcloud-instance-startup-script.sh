#!/bin/bash

set -x

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
sudo apt-get install -y containerd.io docker-ce docker-ce-cli
sudo groupadd docker
sudo usermod -aG docker $USER
newgrp docker
yes | gcloud auth configure-docker

# install docker compose
sudo curl -L "https://github.com/docker/compose/releases/download/1.27.4/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose

# pull repo
git clone --single-branch --branch debug-gcp-process https://github.com/javamonn/literal.git 

# start the containers
# LD_LIBRARY_PATH issue: https://github.com/google-github-actions/setup-gcloud/issues/128
cd ./literal/packages/ebs-automl-highlight-detection-data && LD_LIBRARY_PATH=/usr/local/lib ./scripts/docker-start-production.sh
