#!/usr/bin/env node

const fs = require("fs");
const path = require("path");
const amplifyConfiguration = require("../app/src/main/res/raw/amplifyconfiguration.json");
const awsConfiguration = require("../app/src/main/res/raw/awsconfiguration.json");
const fossPath = path.resolve(__dirname, "../app/src/foss/res/raw");

amplifyConfiguration.auth.plugins.awsCognitoAuthPlugin.Auth.Default.OAuth = undefined;
amplifyConfiguration.auth.plugins.awsCognitoAuthPlugin.CognitoUserPool.Default.AppClientSecret = undefined;
awsConfiguration.Auth.Default.OAuth = undefined;
awsConfiguration.CognitoUserPool.Default.AppClientSecret = undefined;

fs.writeFileSync(
  path.resolve(fossPath, "amplifyconfiguration.json"),
  JSON.stringify(amplifyConfiguration)
);
fs.writeFileSync(
  path.resolve(fossPath, "awsconfiguration.json"),
  JSON.stringify(awsConfiguration)
);
