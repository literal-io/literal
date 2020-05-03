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

  [@react.component]
  let make = (~highlight) => {
    let (updateHighlightMutation, _s, _f) =
      ApolloHooks.useMutation(UpdateHighlightMutation.definition);

    let (textState, setTextState) = React.useState(() => {highlight##text});

    let handleSave = () => {
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
      let _ =
        updateHighlightMutation(~variables, ())
        |> Js.Promise.then_(_ => {
             let _ =
               Webview.(
                 postMessage(WebEvent.make(~type_="ACTIVITY_FINISH"))
               );
             Js.Promise.resolve();
           });
      ();
    };

    let handleClose = () => {
      let _ = Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH")));
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
        "relative",
        "overflow-y-auto",
      ])}>
      <Header>
        <MaterialUi.IconButton
          size=`Small
          edge=`Start
          onClick={_ => handleClose()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-50"]),
            },
          }
          classes=[Root(cn(["p-0"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["w-8", "h-8", "pointer-events-none"])}
            icon=Svg.close
          />
        </MaterialUi.IconButton>
      </Header>
      <div className={cn(["px-6", "pt-4", "pb-24"])}>
        <TextInput.Basic onChange=handleTextChange value=textState />
      </div>
      <FloatingActionButton
        onClick={_ev => handleSave()}
        className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={cn(["w-10", "h-10", "pointer-events-none"])}
          icon=Svg.done_
        />
      </FloatingActionButton>
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
