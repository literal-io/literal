const path = require("path");
const webpack = require("webpack");

module.exports = {
  target: "node",
  mode: process.env.NODE_ENV || "development",
  entry: [path.resolve(__dirname, "./src/Index.bs.js")],
  output: {
    filename: "bundle.js",
    libraryTarget: "umd"
  },
  plugins: [new webpack.EnvironmentPlugin([
    "NODE_ENV",
    "API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT",
    "STORAGE_LITERAL_STORAGE_BUCKETNAME",
    "AUTH_LITERAL_AUTH_USERPOOLID",
    "AUTH_LITERAL_AUTH_USERPOOLWEBCLIENTID"
    "REGION"
  ])]
};
