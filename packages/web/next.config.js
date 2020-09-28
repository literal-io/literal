const R = require("ramda");
const amplifyConfig = require("./amplify/.config/local-env-info.json");

module.exports = {
  devIndicators: {
    autoPrerender: false,
  },
  webpack: (config, { webpack, dev }) => {
    return R.pipe(
      R.assocPath(
        ["resolve", "alias", "apollo-client"],
        require.resolve("apollo-client")
      ),
      R.assocPath(
        ["resolve", "alias", "apollo-cache-inmemory"],
        require.resolve("apollo-cache-inmemory")
      ),
      R.assocPath(
        ["module", "rules"],
        R.append(
          {
            test: /\.svg$/,
            loader: "file-loader",
            options: {
              outputPath: "static",
              esModule: false,
              name: dev ? "[name].[ext]" : "[contenthash].[ext]",
              publicPath: "/_next/static",
            },
          },
          R.pathOr([], ["module", "rules"], config)
        )
      ),
      R.assocPath(
        ["plugins"],
        R.append(
          new webpack.DefinePlugin({
            "process.env.AMPLIFY_ENV": JSON.stringify(amplifyConfig.envName),
          }),
          R.pathOr([], ["plugins"], config)
        )
      )
    )(config);
  },
};
