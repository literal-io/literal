open Styles;

module Data = {
  [@react.component]
  let make = (~highlights) => {
    let activeIdx = 0;
    let handleIdxChange = idx => ();

    <>
      <Containers_NoteHeader
        highlightFragment={
          highlights
          ->Belt.Array.getExn(activeIdx)
          ->(h => h##headerHighlightFragment)
        }
      />
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal
        onIdxChange=handleIdxChange
        initialIdx=0>
        {highlights->Belt.Array.map(h =>
           <ScrollSnapList.Item
             key={h##id} direction=ScrollSnapList.Horizontal>
             <Header />
             <Containers_NoteEditor highlightFragment={h##editorHighlightFragment} />
           </ScrollSnapList.Item>
         )}
      </ScrollSnapList.Container>
    </>;
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

[@react.component]
let make = () => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      QueryRenderers_Notes_GraphQL.ListHighlightsQuery.definition,
    );

  <div className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
    {switch (query) {
     | Data(data) =>
       switch (data##listHighlights->Belt.Option.flatMap(h => h##items)) {
       | Some(highlights) =>
         <Data
           highlights={
             highlights->Belt.Array.keepMap(i => i)
             |> Ramda.sortBy(h => {
                  -. h##createdAt->Js.Date.fromString->Js.Date.valueOf
                })
           }
         />
       | None => <Empty />
       }
     | Loading => <Loading />
     | NoData
     | Error(_) => <Empty />
     }}
  </div>;
};
