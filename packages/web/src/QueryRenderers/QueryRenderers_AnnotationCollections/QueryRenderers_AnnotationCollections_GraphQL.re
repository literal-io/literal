module ListAnnotationCollections = {
  module Query = [%graphql
    {|
    query ListAnnotationCollections($creatorUsername: String!, $nextToken: String) {
      listAnnotationCollections(
        creatorUsername: $creatorUsername,
        limit: 50,
        sortDirection: DESC,
        nextToken: $nextToken
      ) {
        __typename
        nextToken
        items {
          __typename
          id
          type_: type
          ...SourceListItem_GraphQL.AnnotationCollectionFragment.SourceListItem_AnnotationCollection @bsField(name: "sourceListItem")
          ...TagListItem_GraphQL.AnnotationCollectionFragment.TagListItem_AnnotationCollection @bsField(name: "tagListItem")
        }
      }
    }
  |}
  ];

  module CacheConfig = {
    type cacheItem = {
      .
      "__typename": string,
      "type_": array(string),
      "id": string,
      "label": string,
      "total": int,
    };
    type cacheAnnotationCollection = {
      .
      "__typename": string,
      "nextToken": Js.Null.t(string),
      "items": Js.Null.t(array(cacheItem)),
    };
    type t = {
      .
      "__typename": string,
      "listAnnotationCollections": Js.Null.t(cacheAnnotationCollection),
    };
    let makeCacheItem = (~type_, ~id, ~label, ~total) => {
      "__typename": "AnnotationCollection",
      "type_": type_,
      "id": id,
      "label": label,
      "total": total,
    };
    let makeCache = (~items) => {
      "__typename": "Query",
      "listAnnotationCollections":
        Js.Null.return({
          "__typename": "ModelAnnotationCollectionConnection",
          "nextToken": Js.Null.empty,
          "items": Js.Null.return(items),
        }),
    };
  };

  module Cache = {
    include Lib_Apollo_Cache.Make(Query, CacheConfig);

    let setItems =
        (
          cacheQuery: CacheConfig.t,
          newItems: Js.Null.t(array(CacheConfig.cacheItem)),
        )
        : CacheConfig.t => {
      let newItemsQuery = {
        "listAnnotationCollections": Some({"items": Some(newItems)}),
      };

      Ramda.mergeDeepLeft(newItemsQuery, cacheQuery);
    };
  };
};
