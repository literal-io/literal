module ListHighlights = {
  module Query = [%graphql
    {|
      query ListHighlights {
        listHighlights(limit: 100) {
          items {
            id
            createdAt
            ...Containers_NoteEditor_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
            ...Containers_NoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
          }
        }
      }
    |}
  ];

  /**
   * Cache representation is of different structure than what
   * graphql_ppx generates.
   */
  module Raw = {
    [@decco]
    type highlight = {
      id: string,
      createdAt: string,
      text: string,
      [@decco.key "__typename"] typename: string
    };

    [@decco]
    [@bs.deriving accessors]
    type listHighlightsConnection = {
      items: option(array(option(highlight))),
      [@decco.key "__typename"] typename: string
    };
    [@decco]
    [@bs.deriving accessors]
    type t = {listHighlights: option(listHighlightsConnection)};

    let decode = t_decode;
    let encode = t_encode;

    external unsafeAsQuery: Js.Json.t => Query.t = "%identity";
  };

  module CacheReadQuery = ApolloClient.ReadQuery(Query);
  module CacheWriteQuery = ApolloClient.WriteQuery(Query);

  let readCache = client => {
    let query = Query.make();
    let readQueryOptions = ApolloHooks.toReadQueryOptions(query);
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

  let writeCache = (~client, ~data) => {
    let query = Query.make();
    CacheWriteQuery.make(
      ~client,
      ~variables=query##variables,
      ~data=data->Raw.encode->Raw.unsafeAsQuery,
      (),
    );
  };
};
