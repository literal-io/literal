open Styles;
open QueryRenderers_NewNote_GraphQL;

module Data = {
  [@react.component]
  let make = (~highlight) => {
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
      <Containers_NewNoteHeader
        highlightFragment={highlight##headerHighlightFragment}
      />
      <Containers_NewNoteEditor
        highlightFragment={highlight##editorHighlightFragment}
      />
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

[@react.component]
let make = (~highlightId) => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=
        GetNoteQuery.makeVariables(~id=highlightId->Belt.Option.getExn, ()),
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
