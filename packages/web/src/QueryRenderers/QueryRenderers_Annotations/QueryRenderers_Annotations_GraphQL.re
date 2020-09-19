module ListAnnotations = {
  module Query = [%graphql
    {|
      query listAnnotations($creatorUsername: String!) {
        __typename
        listAnnotations(creatorUsername: $creatorUsername, limit: 100) {
          __typename
          items {
            __typename
            id
            created
            ...Containers_AnnotationEditor_Annotation_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
          }
        }
      }
    |}
  ];

  external unsafeFromCache: Js.Json.t => Query.t = "%identity";

  /** FIXME: handle wrapped values **/
  let unsafeToCache = (data: Query.t) => {
    let flattenedAnnotations =
      data##listAnnotations
      ->Belt.Option.flatMap(d => d##items)
      ->Belt.Option.map(annotations => {
          annotations->Belt.Array.keepMap(annotation =>
            annotation->Belt.Option.map(annotation => {
              annotation
              ->Ramda.mergeDeepLeft(annotation##editorAnnotationFragment)
            })
          )
        })
      ->Belt.Option.getWithDefault([||]);
    {
      "listAnnotations": Some({
        "items": Some(flattenedAnnotations),
        "__typename": "ModelAnnotationConnection",
      }),
      "__typename": "Query",
    };
  };

  module CacheReadQuery = ApolloClient.ReadQuery(Query);
  module CacheWriteQuery = ApolloClient.WriteQuery(Query);

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
      cachedResponse->Js.Nullable.toOption->Belt.Option.map(unsafeFromCache)
    };
  };

  /**
   * Data in query format, i.e. with fragments. Note that this is note the
   * type of data once it hits the cache, but is more typesafe to work with
   * when manually creating new cache values.
   */
  let writeCache = (~query, ~client, ~data, ()) => {
    CacheWriteQuery.make(~client, ~variables=query##variables, ~data, ());
  };

  /**
   * Data in cache format, i.e. everything inlined on GraphQL types. Not a good
   * way to enforce type safety.
   */
  let unsafeWriteCache = (~query, ~client, ~data, ()) =>
    writeCache(
      ~query,
      ~client,
      ~data=unsafeFromCache(data)
    );
};
