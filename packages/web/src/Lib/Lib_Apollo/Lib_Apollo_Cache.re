module type CacheConfig = {type t;};

module Make = (Query: ReasonApolloTypes.Config, CacheConfig: CacheConfig) => {
  module CacheReadQuery = ApolloClient.ReadQuery(Query);
  module CacheWriteQuery = ApolloClient.WriteQuery(Query);

  external unsafeToCache: Js.Json.t => CacheConfig.t = "%identity";
  external unsafeCacheToJson: CacheConfig.t => Js.Json.t = "%identity";

  /**
     * FIXME: Return type is not actually Query.t, as Apollo cache
     * does not persist fragment alias fields -- everything is
     * inlined on the root type.
     */
  let readCache = (~query, ~client, ()) => {
    let readQueryOptions =
      CacheReadQuery.{
        query: ApolloClient.gql(. query##query),
        variables: Js.Nullable.fromOption(Some(query##variables)),
      };
    switch (CacheReadQuery.readQuery(client, readQueryOptions)) {
    | exception _ => None
    | cachedResponse =>
      cachedResponse->Js.Nullable.toOption->Belt.Option.map(unsafeToCache)
    };
  };

  /**
     * Data in cache format, i.e. everything inlined on GraphQL types. Not a good
     * way to enforce type safety.
     */
  external unsafeFromCache: CacheConfig.t => Query.t = "%identity";
  let writeCache = (~query, ~client, ~data: CacheConfig.t, ()) =>
    CacheWriteQuery.make(
      ~client,
      ~variables=query##variables,
      ~data=unsafeFromCache(data),
      (),
    );
};
