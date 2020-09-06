const path = require("path");
const webpack = require("webpack");

module.exports = {
  target: "node",
  mode: process.env.NODE_ENV || "development",
  entry: [path.resolve(__dirname, "./src/Index.bs.js")],
  node: {
    __dirname: false,
  },
  output: {
    filename: "bundle.js",
    libraryTarget: "umd",
    path: path.resolve(__dirname, "dist"),
  },
  externals: {
    "@google-cloud/vision": "@google-cloud/vision",
  },
  plugins: [
    new webpack.EnvironmentPlugin(
      process.env.NODE_ENV === "production"
        ? [
            "AUTH_LITERALAUTH_USERPOOLWEBCLIENTID",
            "HIGHLIGHT_BOUNDING_BOX_DETECTOR_API",
          ]
        : [
            "NODE_ENV",
            "API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT",
            "STORAGE_LITERALSTORAGE_BUCKETNAME",
            "AUTH_LITERALAUTH_USERPOOLID",
            "AUTH_LITERALAUTH_USERPOOLWEBCLIENTID",
            "HIGHLIGHT_BOUNDING_BOX_DETECTOR_API",
            "REGION",
          ]
    ),
  ],
};
