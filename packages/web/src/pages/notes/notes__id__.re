open Styles;

module Data = {
  module UpdateHighlightMutation = [%graphql
    {|
    mutation UpdateHighlight($input: UpdateHighlightInput!) {
      updateHighlight(input: $input) {
        id
      }
    }
  |}
  ];

  let handleSave =
    Lodash.debounce2(
      (.
        variables,
        updateHighlightMutation:
          ApolloHooks.Mutation.mutation(UpdateHighlightMutation.t),
      ) => {
        let _ = updateHighlightMutation(~variables, ());
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
          let _ = handleSave(. variables, updateHighlightMutation);

          None;
        },
        [|textState|],
      );

    let _ =
      React.useEffect0(() => {
        Some(
          () => {
            let _ = Lodash.flush2(handleSave);
            ();
          },
        )
      });

    let handleBack = () => {
      let _ = Next.Router.back();
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
      <Header>
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
            className={cn(["w-6", "h-6", "pointer-events-none"])}
            icon=Svg.back
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
