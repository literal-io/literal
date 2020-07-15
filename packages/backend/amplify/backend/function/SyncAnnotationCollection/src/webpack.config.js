const path = require("path");
const webpack = require("webpack");

module.exports = {
  target: "node",
  mode: process.env.NODE_ENV || "development",
  entry: [path.resolve(__dirname, "./src/Index.bs.js")],
  output: {
    filename: "bundle.js",
    libraryTarget: "umd",
    path: path.resolve(__dirname, "./dist"),
  },
  plugins: [
    new webpack.EnvironmentPlugin([
      "NODE_ENV",
      "API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT",
      "STORAGE_LITERALSTORAGE_BUCKETNAME",
      "AUTH_LITERALAUTH_USERPOOLID",
      "AUTH_LITERALAUTH_USERPOOLWEBCLIENTID",
      "REGION",
    ]),
  ],
};
