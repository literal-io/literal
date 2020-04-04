const R = require("ramda");

module.exports = {
  webpack: (config, { webpack }) => {
    return R.pipe(
      R.assocPath(
        ["resolve", "alias", "apollo-client"],
        require.resolve("apollo-client")
      ),
      R.assocPath(
        ["resolve", "alias", "apollo-cache-inmemory"],
        require.resolve("apollo-cache-inmemory")
      )
    )(config);
  },
};
