module GetAnnotationCollection = {
  module Query = [%graphql
    {|
      query getAnnotationCollection($creatorUsername: String!, $id: String!, $nextToken: String) {

        getAgent(username: $creatorUsername) {
          id
        }

        getAnnotationCollection(creatorUsername: $creatorUsername, id: $id) {
          ...Containers_AnnotationCollectionHeader_GraphQL.GetAnnotationCollectionFragment.AnnotationCollectionHeader_AnnotationCollection @bsField(name: "annotationCollectionHeader")
          __typename
          first {
            __typename
            items(limit: 100, sortDirection: DESC, nextToken: $nextToken) {
              __typename
              nextToken
              items {
                __typename
                annotation {
                  __typename
                  created
                  id
                  ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
                  ...Containers_AnnotationCollectionHeader_GraphQL.GetAnnotationFragment.AnnotationCollectionHeader_Annotation @bsField(name: "annotationCollectionHeader")
                  ...Containers_NewTagInput_GraphQL.GetAnnotationFragment.NewTagInputAnnotation @bsField(name: "newTagInputFragment")
                }
              }
            }
          }
        }
      }
    |}
  ];

  type cacheAnnotation = {
    .
    "__typename": string,
    "created": Js.Null.t(Js.Json.t),
    "modified": Js.Null.t(Js.Json.t),
    "id": string,
    "body":
      Js.Null.t(
        array(
          Js.Null.t({
            .
            "__typename": string,
            "id": string,
            "value": string,
            "purpose": Js.Null.t(array(string)),
            "format": Js.Null.t(string),
            "language": Js.Null.t(string),
            "processingLanguage": Js.Null.t(string),
            "accessibility": Js.Null.t(array(string)),
            "rights": Js.Null.t(array(string)),
            "textDirection": Js.Null.t(string),
          }),
        ),
      ),
    "target":
      array(
        Js.Null.t({
          .
          "__typename": string,
          "value": Js.Null.t(string),
          "textualTargetId": Js.Null.t(string),
          "externalTargetId": Js.Null.t(string),
          "format": Js.Null.t(string),
          "language": Js.Null.t(string),
          "processingLanguage": Js.Null.t(string),
          "accessibility": Js.Null.t(array(string)),
          "rights": Js.Null.t(array(string)),
          "textDirection": Js.Null.t(string),
        }),
      ),
  };

  type cacheGetAnnotationCollection = {
    .
    "__typename": string,
    "label": string,
    "first":
      Js.Null.t({
        .
        "__typename": string,
        "items":
          Js.Null.t({
            .
            "__typename": string,
            "nextToken": Js.Null.t(string),
            "items":
              Js.Null.t(
                array({
                  .
                  "__typename": string,
                  "annotation": Js.Json.t,
                }),
              ),
          }),
      }),
  };

  /**
   * Apollo cache representation of the above query. Note that this will
   * need to be manually updated as the query changes.
   */
  type cache = {
    .
    "__typename": string,
    "getAgent":
      Js.Null.t({
        .
        "__typename": string,
        "id": string,
      }),
    "getAnnotationCollection": Js.Null.t(cacheGetAnnotationCollection),
  };

  let parsedTextualBodyToCache = d => {
    "__typename": "TextualBody",
    "id": d##id,
    "value": d##value,
    "purpose":
      d##purpose
      ->Belt.Option.map(d =>
          d->Belt.Array.map(Lib_GraphQL_Motivation.toString)
        )
      ->Js.Null.fromOption,
    "format":
      d##format
      ->Belt.Option.map(d =>
          switch (d) {
          | `TEXT_PLAIN => "TEXT_PLAIN"
          | `TEXT_HTML => "TEXT_HTML"
          }
        )
      ->Js.Null.fromOption,
    "language":
      d##language
      ->Belt.Option.map(Lib_GraphQL_Language.toString)
      ->Js.Null.fromOption,
    "processingLanguage":
      d##processingLanguage
      ->Belt.Option.map(Lib_GraphQL_Language.toString)
      ->Js.Null.fromOption,
    "accessibility": d##accessibility->Js.Null.fromOption,
    "rights": d##rights->Js.Null.fromOption,
    "textDirection":
      d##textDirection
      ->Belt.Option.map(Lib_GraphQL_TextDirection.toString)
      ->Js.Null.fromOption,
  };

  let parsedAnnotationToCache = (annotation): cacheAnnotation => {
    "__typename": "Annotation",
    "created": annotation##created->Js.Null.fromOption,
    "modified": annotation##modified->Js.Null.fromOption,
    "id": annotation##id,
    "body":
      annotation##body
      ->Belt.Option.map(d =>
          d
          ->Belt.Array.keepMap(d =>
              switch (d) {
              | `TextualBody(d) => Some(d)
              | `Nonexhaustive => None
              }
            )
          ->Belt.Array.map(body =>
              body->parsedTextualBodyToCache->Js.Null.return
            )
        )
      ->Js.Null.fromOption,
    "target":
      annotation##target
      ->Belt.Array.map(d =>
          switch (d) {
          | `TextualTarget(d) =>
            {
              "__typename": "TextualTarget",
              "value": Js.Null.return(d##value),
              "textualTargetId": d##textualTargetId->Js.Null.return,
              "externalTargetId": Js.Null.empty,
              "format":
                d##format
                ->Belt.Option.map(Lib_GraphQL_Format.toString)
                ->Js.Null.fromOption,
              "language":
                d##language
                ->Belt.Option.map(Lib_GraphQL_Language.toString)
                ->Js.Null.fromOption,
              "processingLanguage":
                d##processingLanguage
                ->Belt.Option.map(Lib_GraphQL_Language.toString)
                ->Js.Null.fromOption,
              "accessibility": d##accessibility->Js.Null.fromOption,
              "rights": d##rights->Js.Null.fromOption,
              "textDirection":
                d##textDirection
                ->Belt.Option.map(Lib_GraphQL_TextDirection.toString)
                ->Js.Null.fromOption,
            }
            ->Js.Null.return
          | _ => Js.Null.empty
          }
        ),
  };

  external unsafeToCache: Js.Json.t => cache = "%identity";
  external unsafeCacheToJson: cache => Js.Json.t = "%identity";

  module CacheReadQuery = ApolloClient.ReadQuery(Query);
  module CacheWriteQuery = ApolloClient.WriteQuery(Query);

  let setAnnotationPageItems =
      (
        cacheQuery: cache,
        newItems:
          Js.Null.t(
            array({
              .
              "annotation": Js.Json.t,
              "__typename": string,
            }),
          ),
      )
      : cache => {
    let newItemsQuery = {
      "getAnnotationCollection":
        Some({
          "__typename": "AnnotationCollection",
          "first":
            Some({
              "__typename": "AnnotationPage",
              "items":
                Some({
                  "__typename": "ModelAnnotationPageItemConnection",
                  "items": Some(newItems),
                }),
            }),
        }),
    };
    Ramda.mergeDeepLeft(newItemsQuery, cacheQuery);
  };

  let setGetAnnotationCollection =
      (
        cacheQuery: cache,
        getAnnotationCollection: cacheGetAnnotationCollection,
      ) => {
    let newQuery = {
      "getAnnotationCollection": Some(getAnnotationCollection),
    };

    Ramda.mergeDeepLeft(newQuery, cacheQuery);
  };

  // setter to update next token
  let setNextToken = (cacheQuery: cache, nextToken: Js.Null.t(string)): cache => {
    let newQuery = {
      "getAnnotationCollection":
        Some({"first": Some({"items": Some({"nextToken": nextToken})})}),
    };
    Ramda.mergeDeepLeft(newQuery, cacheQuery);
  };

  let makeCache = (~label, ~annotations): cache => {
    "__typename": "Query",
    "getAnnotationCollection":
      Js.Null.return({
        "__typename": "AnnotationCollection",
        "label": label,
        "first":
          Js.Null.return({
            "__typename": "AnnotationPage",
            "items":
              Js.Null.return({
                "__typename": "ModelAnnotationPageItemConnection",
                "nextToken": Js.Null.empty,
                "items":
                  annotations
                  ->Belt.Array.map(a =>
                      {"__typename": "AnnotationPageItem", "annotation": a}
                    )
                  ->Js.Null.return,
              }),
          }),
      }),
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
      cachedResponse->Js.Nullable.toOption->Belt.Option.map(unsafeToCache)
    };
  };

  /**
   * Data in cache format, i.e. everything inlined on GraphQL types. Not a good
   * way to enforce type safety.
   */
  external unsafeFromCache: cache => Query.t = "%identity";
  let writeCache = (~query, ~client, ~data: cache, ()) =>
    CacheWriteQuery.make(
      ~client,
      ~variables=query##variables,
      ~data=unsafeFromCache(data),
      (),
    );
};
