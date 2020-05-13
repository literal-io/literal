open Styles;

module Data = {
  [@react.component]
  let make = (~highlights, ~initialHighlightId, ~onHighlightIdChange) => {
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

module Empty = {
  [@react.component]
  let make = () => <span className={cn(["text-white"])}> {React.string("Empty...")} </span>
};

module Loading = {
  [@react.component]
  let make = () => React.string("Loading...");
};

[@react.component]
let make = (~highlightId, ~onHighlightIdChange) => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      QueryRenderers_Notes_GraphQL.ListHighlightsQuery.definition,
    );

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
