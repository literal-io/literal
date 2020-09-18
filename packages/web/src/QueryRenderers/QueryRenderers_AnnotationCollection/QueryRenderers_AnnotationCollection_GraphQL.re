module GetAnnotationCollection = {
  module Query = [%graphql
    {|
      query getAnnotationCollection($creatorUsername: String!, $id: String!) {
        getAnnotationCollection(creatorUsername: $creatorUsername, id: $id) {
          ...Containers_AnnotationCollectionHeader_GraphQL.GetAnnotationCollectionFragment.AnnotationCollectionHeader_AnnotationCollection @bsField(name: "annotationCollectionHeader")
          first {
            items(limit: 100) {
              items {
                annotation {
                  created
                  id
                  ...Containers_AnnotationEditor_Annotation_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
                  ...Containers_AnnotationCollectionHeader_GraphQL.GetAnnotationFragment.AnnotationCollectionHeader_Annotation @bsField(name: "annotationCollectionHeader")
                }
              }
            }
          }
        }
      }
    |}
  ];

  external fromCache: Js.Json.t => Query.t = "%identity";

  module CacheReadQuery = ApolloClient.ReadQuery(Query);
  module CacheWriteQuery = ApolloClient.WriteQuery(Query);

  // setter to update items
  let setCacheItems = (cacheQuery, newItems) => {
    let newItemsQuery = {
      "getAnnotationCollection":
        Some({"first": Some({"items": Some({"items": Some(newItems)})})}),
    };
    Ramda.mergeDeepLeft(newItemsQuery, cacheQuery);
  };

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
      cachedResponse->Js.Nullable.toOption->Belt.Option.map(fromCache)
    };
  };

  let writeCache = (~query, ~client, ~data, ()) => {
    CacheWriteQuery.make(~client, ~variables=query##variables, ~data, ());
  };
};
