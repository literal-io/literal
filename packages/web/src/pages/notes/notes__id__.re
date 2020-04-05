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

    let (editorState, setEditorState) =
      React.useState(() => {
        let contentState = Draft.ContentState.createFromText(highlight##text);
        let decorator =
          Draft.Decorator.makeComposite([|
            HighlightTextDecorator.decoratorInput,
          |]);
        Draft.EditorState.makeWithContent(contentState, Some(decorator));
      });

    let handleSave = () => {
      let variables =
        UpdateHighlightMutation.makeVariables(
          ~input={
            "id": highlight##id,
            "text":
              editorState
              ->Draft.EditorState.getCurrentContent
              ->Draft.ContentState.convertToRaw
              ->Draft.ContentState.blocks
              ->Belt.Array.map(Draft.Block.text)
              ->Js.Array2.joinWith("\n")
              ->Js.Option.some,
            "createdAt": None,
            "note": None,
            "highlightScreenshotId": None,
          },
          (),
        );
      let _ = updateHighlightMutation(~variables, ());
      ();
    };

    let handleChange = nextEditorState => setEditorState(_ => nextEditorState);

    <div
      className={cn([
        "w-full",
        "h-full",
        "bg-black",
        "px-6",
        "flex",
        "flex-col",
        "relative",
      ])}>
      <Header title="Create" />
      <div className={cn(["border-white", "border-b", "py-2"])}>
        <TextEditor editorKey="highlight" editorState onChange=handleChange />
      </div>
      <FloatingActionButton
        onClick={_ev => handleSave()}
        className={cn(["absolute", "right-0", "bottom-0", "m-6", "z-10"])}>
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
  let router = Next.useRouter();

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
