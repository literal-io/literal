const R = require("ramda");
const RemarkHTML = require("remark-html");
const amplifyConfig = require("./amplify/.config/local-env-info.json");

const commitHash = require("child_process")
  .execSync("git rev-parse --short HEAD")
  .toString()
  .trim();

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
        R.concat(
          [
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
            {
              test: /\.md$/,
              use: [
                {
                  loader: "html-loader",
                },
                {
                  loader: "remark-loader",
                  options: {
                    remarkOptions: {
                      plugins: [RemarkHTML],
                    },
                  },
                },
              ],
            },
          ],
          R.pathOr([], ["module", "rules"], config)
        )
      ),
      R.assocPath(
        ["plugins"],
        R.append(
          new webpack.DefinePlugin({
            "process.env.AMPLIFY_ENV": JSON.stringify(amplifyConfig.envName),
            "process.env.AMPLITUDE_API_KEY": JSON.stringify(
              "390bccc68484055cef405613213849e5"
            ),
            "process.env.SENTRY_DSN": JSON.stringify(
              "https://2ca22f543045459bbe86e915bb94953e@o455440.ingest.sentry.io/5447049"
            ),
            "process.env.WEB_APP_VERSION": JSON.stringify(commitHash),
          }),
          R.pathOr([], ["plugins"], config)
        )
      )
    )(config);
  },
};
