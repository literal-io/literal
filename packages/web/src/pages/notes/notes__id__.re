open Styles;

module GetNoteQuery = [%graphql
  {|
    query GetHighlight($id: ID!) {
      getHighlight(id: $id) {
        id
        text
        note
      }
    }
  |}
];

external jsonToHighlight: Js.Json.t => GetNoteQuery.t = "%identity";

module Data = {
  module UpdateHighlightMutation = [%graphql
    {|
    mutation UpdateHighlight($input: UpdateHighlightInput!) {
      updateHighlight(input: $input) {
        id
        text
      }
    }
  |}
  ];

  module HighlightCacheReadQuery = ApolloClient.ReadQuery(GetNoteQuery);
  module HighlightCacheWriteQuery = ApolloClient.WriteQuery(GetNoteQuery);

  let handleSave =
    Lodash.debounce3(
      (.
        variables,
        updateHighlightMutation:
          ApolloHooks.Mutation.mutation(UpdateHighlightMutation.t),
        onUpdate,
      ) => {
        let _ =
          updateHighlightMutation(~variables, ())
          |> Js.Promise.then_(onUpdate);
        ();
      },
      500,
    );

  [@react.component]
  let make = (~highlight) => {
    let (updateHighlightMutation, _s, _f) =
      ApolloHooks.useMutation(UpdateHighlightMutation.definition);
    let (textState, setTextState) = React.useState(() => {highlight##text});
    let _ =
      React.useEffect1(
        () => {
          let variables =
            UpdateHighlightMutation.makeVariables(
              ~input={
                "id": highlight##id,
                "text": textState->Js.Option.some,
                "createdAt": None,
                "note": None,
                "highlightScreenshotId": None,
              },
              (),
            );
          let onUpdate =
              (
                result: ApolloHooks.Mutation.result(UpdateHighlightMutation.t),
              ) => {
            let _ =
              switch (result) {
              | Data(data) =>
                switch (data##updateHighlight) {
                | Some(h) =>
                  let query = GetNoteQuery.make(~id=h##id, ());
                  let readQueryOptions =
                    ApolloHooks.toReadQueryOptions(query);
                  let _ =
                    switch (
                      HighlightCacheReadQuery.readQuery(
                        Provider.client,
                        readQueryOptions,
                      )
                    ) {
                    | exception _ => ()
                    | cachedResponse =>
                      switch (cachedResponse->Js.Nullable.toOption) {
                      | None => ()
                      | Some(cachedHighlight) =>
                        let highlight = jsonToHighlight(cachedHighlight);
                        let updatedHighlight = [%bs.raw
                          {| {
                          ...highlight,
                          ...h
                        } |}
                        ];
                        HighlightCacheWriteQuery.make(
                          ~client=Provider.client,
                          ~variables=query##variables,
                          ~data=updatedHighlight,
                          (),
                        );
                        ();
                      }
                    };
                  ();
                | None => ()
                }
              | Error(error) => ()
              | NoData => ()
              | _ => ()
              };
            Js.Promise.resolve();
          };

          let _ = handleSave(. variables, updateHighlightMutation, onUpdate);

          None;
        },
        [|textState|],
      );

    let _ =
      React.useEffect0(() => {
        Some(
          () => {
            let _ = Lodash.flush3(handleSave);
            ();
          },
        )
      });

    let handleBack = () => {
      let _ = Next.Router.back();
      ();
    };

    let handleDelete = () => {
      ();
    };

    let handleTextChange = s => setTextState(_ => s);

    <div
      className={cn([
        "w-full",
        "h-full",
        "bg-black",
        "flex",
        "flex-col",
        "overflow-y-auto",
      ])}>
      <Header className={cn(["justify-between"])}>
        <MaterialUi.IconButton
          size=`Small
          edge=`Start
          onClick={_ => handleBack()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-50"]),
            },
          }
          classes=[Root(cn(["p-0", "ml-1"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn([
              "w-6",
              "h-6",
              "pointer-events-none",
              "opacity-75",
            ])}
            icon=Svg.back
          />
        </MaterialUi.IconButton>
        <MaterialUi.IconButton
          size=`Small
          edge=`End
          onClick={_ => handleDelete()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-50"]),
            },
          }
          classes=[Root(cn(["p-0", "ml-1"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["pointer-events-none", "opacity-75"])}
            style={ReactDOMRe.Style.make(
              ~width="1.75rem",
              ~height="1.75rem",
              (),
            )}
            icon=Svg.delete
          />
        </MaterialUi.IconButton>
      </Header>
      <div className={cn(["px-6", "py-4"])}>
        <TextInput.Basic onChange=handleTextChange value=textState />
      </div>
    </div>;
  };
};

module Empty = {
  [@react.component]
  let make = () => React.string("Not Found...");
};

module Loading = {
  [@react.component]
  let make = () => React.string("Loading...");
};

[@decco]
type routeParams = {id: string};

[@react.component]
let default = () => {
  let router = Next.Router.useRouter();

  let postId =
    switch (routeParams_decode(router.Next.query)) {
    | Ok(p) => Some(p.id)
    | _ => None
    };

  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=
        GetNoteQuery.makeVariables(
          ~id=postId->Belt.Option.getWithDefault(""),
          (),
        ),
      GetNoteQuery.definition,
    );

  switch (query) {
  | Data(data) =>
    switch (data##getHighlight) {
    | Some(highlight) => <Data highlight />
    | None => <Empty />
    }
  | Loading => <Loading />
  | NoData
  | Error(_) => <Empty />
  };
};
