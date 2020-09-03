module ListAnnotations = {
  module Query = [%graphql
    {|
      query listAnnotations($creatorUsername: String!) {
        listAnnotations(creatorUsername: $creatorUsername, limit: 100) {
          items {
            id
            created
            ...Containers_NoteEditor_Notes_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
            ...Containers_NoteHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
          }
        }
      }
    |}
  ];

  /**
   * Cache representation is of different structure than what
   * graphql_ppx generates.
   */
  /** TODO: Fix cache
  module Raw = {
    [@decco]
    type connection('a) = {
      items: option(array(option('a))),
      [@decco.key "__typename"]
      typename: string,
    };

    [@decco]
    type tag = {
      id: string,
      text: string,
      [@decco.key "__typename"]
      typename: string,
    };
    let makeTag = (~id, ~text) => {id, text, typename: "Tag"};

    [@decco]
    type highlightTag = {
      id: string,
      createdAt: string,
      tag,
      [@decco.key "__typename"]
      typename: string,
    };
    let makeHighlightTag = (~id, ~createdAt, ~tag) => {
      id,
      createdAt,
      tag,
      typename: "HighlightTag",
    };

    let highlightTagConnectionItems =
        (highlightTagConnection: connection(highlightTag)) =>
      highlightTagConnection.items;

    [@decco]
    type highlight = {
      id: string,
      createdAt: string,
      text: string,
      tags: option(connection(highlightTag)),
      [@decco.key "__typename"]
      typename: string,
    };
    let makeHighlight = (~id, ~createdAt, ~text, ~tags) => {
      id,
      createdAt,
      text,
      tags,
      typename: "Highlight",
    };
    let makeHighlightTagsConnection = (~tags) => {
      items: tags,
      typename: "ModelHighlightTagConnection",
    };

    [@decco]
    type profile = {
      id: string,
      owner: string,
      isOnboarded: bool,
      [@decco.key "__typename"]
      typename: string,
    };

    let highlightConnectionItems =
        (highlightConnection: connection(highlight)) =>
      highlightConnection.items;

    let makeHighlightConnection = (~highlights) => {
      items: highlights,
      typename: "ModelHighlightConnection",
    };

    [@decco]
    [@bs.deriving accessors]
    type t = {
      listHighlights: option(connection(highlight)),
      getProfile: option(profile),
    };

    let decode = t_decode;
    let encode = t_encode;

    external unsafeAsQuery: Js.Json.t => Query.t = "%identity";
  };

  module CacheReadQuery = ApolloClient.ReadQuery(Query);
  module CacheWriteQuery = ApolloClient.WriteQuery(Query);

  let readCache = (~query, ~client, ()) => {
    let readQueryOptions =
      CacheReadQuery.{
        query: ApolloClient.gql(. query##query),
        variables: Js.Nullable.fromOption(Some(query##variables)),
      };
    switch (CacheReadQuery.readQuery(client, readQueryOptions)) {
    | exception _ => None
    | cachedResponse =>
      cachedResponse
      ->Js.Nullable.toOption
      ->Belt.Option.flatMap(j => {
          switch (Raw.decode(j)) {
          | Ok(c) => Some(c)
          | Error(e) =>
            let _ = Error.report(Error.DeccoDecodeError(e));
            None;
          }
        })
    };
  };

  let writeCache = (~query, ~client, ~data, ()) => {
    CacheWriteQuery.make(
      ~client,
      ~variables=query##variables,
      ~data=data->Raw.encode->Raw.unsafeAsQuery,
      (),
    );
  };
  **/
};
