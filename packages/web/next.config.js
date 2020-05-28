const R = require("ramda");

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
      )
    )(config);
  },
};
