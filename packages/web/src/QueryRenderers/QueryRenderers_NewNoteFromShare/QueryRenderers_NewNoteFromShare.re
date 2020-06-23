open Styles;
open QueryRenderers_NewNoteFromShare_GraphQL;

let styles = [%raw "require('./QueryRenderers_NewNoteFromShare.module.css')"];

module Data = {
  [@react.component]
  let make = (~highlight, ~currentUser) => {
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
      <Containers_NewNoteFromShareHeader
        currentUser
        highlightFragment={highlight##headerHighlightFragment}
      />
      <Containers_NoteEditor_NewFromShare
        currentUser
        highlightFragment={highlight##editorHighlightFragment}
      />
    </div>;
  };
};

module Loading = {
  let lineCount = 8;

  [@react.component]
  let make = () => {
    let (margins, _setMargins) =
      React.useState(_ => {
        let margins = [|"", "", "mr-1", "mr-2", "mr-3", "mr-4"|];
        Belt.Array.make(lineCount, 0)
        ->Belt.Array.map(_ =>
            margins[Js.Math.random_int(0, Js.Array2.length(margins))]
          );
      });

    <>
      <Containers_NewNoteFromShareHeader />
      <div className={cn(["px-6", "pb-4", "pt-16"])}>
        {Belt.Array.make(lineCount, 0)
         ->Belt.Array.mapWithIndex((idx, _) => {
             <MaterialUiLab.Skeleton
               key={string_of_int(idx)}
               variant=`text
               classes={
                 "root": cn([styles##skeleton, "mb-1", "h-8", margins[idx]]),
               }
             />
           })
         ->React.array}
      </div>
      <FloatingActionButton
        className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}
        disabled={true}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={cn(["w-10", "h-10", "pointer-events-none"])}
          icon=Svg.done_
        />
      </FloatingActionButton>
    </>;
  };
};

module Empty = {
  [@react.component]
  let make = () => React.string("Not Found...");
};

[@react.component]
let make = (~highlightId, ~currentUser, ~rehydrated) => {
  let (isLoaded, setIsLoaded) = React.useState(_ => false);
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=GetNoteQuery.makeVariables(~id=highlightId, ()),
      ~pollInterval=isLoaded ? 0 : 500,
      ~skip=!rehydrated,
      GetNoteQuery.definition,
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (query) {
          | Data(_) => setIsLoaded(_ => true)
          | _ => ()
          };
        None;
      },
      [|query|],
    );

  switch (query, /*rehydrated*/ false) {
  | (Loading, _)
  | (_, false) => <Loading />
  | (Data(data), _) =>
    switch (data##getHighlight) {
    | Some(highlight) => <Data highlight currentUser />
    | None => <Empty />
    }
  | (NoData, true)
  | (Error(_), true) => <Empty />
  };
};
