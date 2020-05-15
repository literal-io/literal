open Styles;
open QueryRenderers_Notes_GraphQL;

module Data = {
  [@react.component]
  let make = (~highlights, ~initialHighlightId, ~onHighlightIdChange) => {
    Js.log2("parsed cache", ListHighlights.readCache(Provider.client));

    let (activeIdx, setActiveIdx) =
      React.useState(() =>
        switch (initialHighlightId) {
        | Some(initialHighlightId) =>
          highlights
          ->Belt.Array.getIndexBy(h => h##id === initialHighlightId)
          ->Belt.Option.getWithDefault(0)
        | None => 0
        }
      );
    let activeHighlight = highlights->Belt.Array.getExn(activeIdx);

    let _ =
      React.useEffect1(
        () => {
          let _ = onHighlightIdChange(activeHighlight##id);
          None;
        },
        [|activeHighlight|],
      );

    let handleIdxChange = idx => setActiveIdx(_ => idx);

    <>
      <Containers_NoteHeader
        highlightFragment={activeHighlight##headerHighlightFragment}
      />
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal
        onIdxChange=handleIdxChange
        initialIdx=activeIdx>
        {highlights->Belt.Array.map(h =>
           <ScrollSnapList.Item
             key={h##id} direction=ScrollSnapList.Horizontal>
             <Containers_NoteEditor
               highlightFragment={h##editorHighlightFragment}
             />
           </ScrollSnapList.Item>
         )}
      </ScrollSnapList.Container>
    </>;
  };
};

module Loading = {
  [@react.component]
  let make = () => React.string("Loading...");
};

module Empty = {
  [@react.component]
  let make = () => {
    let (createHighlightMutation, _s, _f) =
      ApolloHooks.useMutation(CreateHighlightMutation.definition);
    let _ =
      React.useEffect0(() => {
        let onboardingNotes = [|
          "Welcome to Literal.\n\nLiteral is an annotation management system. Annotations from books that you read will appear here.\n\nScroll right to learn more.",
          "Literal is best used to capture annotations while you're reading. Literal is agnostic about where and how you currently read text, and supports annotations made within a web browser, PDF reader, and more.\n\nTo create a note, highlight the text and use your device's share dialog to share it to the Literal application. If there is no share dialog, screenshot the highlight and share the screenshot to the Literal application.\n\nScroll right to learn more.",
          "Annotations are organized primarily based on tags and bi-directional links between tags in order to retain context and build connections.\n\nIf you have any questions, reach out to hello@literal.io.\n\nOnce you've created some annotations, feel free to delete these introductory annotations.",
        |];

        let p =
          onboardingNotes
          ->Belt.Array.reverse
          ->Belt.Array.map(text => {
              let variables =
                CreateHighlightMutation.makeVariables(
                  ~input={
                    "id": Uuid.makeV4(),
                    "text": text,
                    "note": None,
                    "highlightScreenshotId": None,
                    "createdAt":
                      Js.Date.make() |> Js.Date.toISOString |> Js.Option.some,
                  },
                  (),
                );
              createHighlightMutation(~variables, ());
            })
          ->Js.Promise.all
          |> Js.Promise.then_(res => {
               Js.log2("complete", res);
               Js.Promise.resolve();
             });
        None;
      });

    <Loading />;
  };
};

[@react.component]
let make = (~highlightId, ~onHighlightIdChange) => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(ListHighlights.Query.definition);

  <div className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
    {switch (query) {
     | Data(data) =>
       switch (data##listHighlights->Belt.Option.flatMap(h => h##items)) {
       | Some(highlights) when Array.length(highlights) > 0 =>
         <Data
           onHighlightIdChange
           initialHighlightId=highlightId
           highlights={
             highlights->Belt.Array.keepMap(i => i)
             |> Ramda.sortBy(h => {
                  -. h##createdAt->Js.Date.fromString->Js.Date.valueOf
                })
           }
         />
       | _ => <Empty />
       }
     | Loading => <Loading />
     | NoData
     | Error(_) => <Empty />
     }}
  </div>;
};
