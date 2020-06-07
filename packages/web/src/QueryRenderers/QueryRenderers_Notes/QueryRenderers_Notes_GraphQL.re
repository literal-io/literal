module ListHighlights = {
  module Query = [%graphql
    {|
      query listHighlights($owner: String!) {
        getProfile(owner: $owner) {
          id
          isOnboarded
          ...Containers_Onboarding_GraphQL.GetProfileFragment.OnboardingProfileFragment @bsField(name: "onboardingProfileFragment")
        }
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
      typename: "ModelHighlightConnection"
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
    let readQueryOptions = ApolloHooks.toReadQueryOptions(query);
    switch (CacheReadQuery.readQuery(client, readQueryOptions)) {
    | exception _ => None
    | cachedResponse =>
      cachedResponse
      ->Js.Nullable.toOption
      ->Belt.Option.flatMap(j => {
          Js.log2("cachedQuery", j);
          switch (Raw.decode(j)) {
          | Ok(c) => Some(c)
          | Error(e) =>
            let _ = Error.report(Error.DeccoDecodeError(e));
            None;
          };
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
};
