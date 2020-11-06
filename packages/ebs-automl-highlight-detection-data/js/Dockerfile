FROM thyrlian/android-sdk

# install android deps needed by appium
RUN sdkmanager --update && \
  sdkmanager \
    "platform-tools" \
    "platforms;android-30" \
    "build-tools;30.0.2" && \
  /opt/license_accepter.sh

# install nvm
ENV NVM_DIR /root/.nvm
ENV NODE_VERSION 12.19.0
RUN apt-get update && apt-get install -y curl
RUN curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.36.0/install.sh | bash \
    && . $NVM_DIR/nvm.sh \
    && nvm install $NODE_VERSION \
    && nvm alias default $NODE_VERSION \
    && nvm use default

ENV NODE_PATH $NVM_DIR/versions/node/v$NODE_VERSION/lib/node_modules
ENV PATH      $NVM_DIR/versions/node/v$NODE_VERSION/bin:$PATH

ADD ./ ./ebs-automl-highlight-detection-data

WORKDIR ./ebs-automl-highlight-detection-data 
RUN npm install

ENTRYPOINT npm run start
