open Styles;
open QueryRenderers_NewNoteFromShare_GraphQL;

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
  [@react.component]
  let make = () => {
    <>
      <Containers_NewNoteFromShareHeader />
      <TextInput_Loading className={cn(["px-6", "pb-4", "pt-16"])} />
      <FloatingActionButton
        className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}
        disabled=true>
        <MaterialUi.CircularProgress
          size={`Int(26)}
          classes=[ColorPrimary(cn(["text-black"]))]
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
let make = (~highlightId, ~authentication: CurrentUserInfo.state, ~rehydrated) => {
  let (isLoaded, setIsLoaded) = React.useState(_ => false);
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=GetNoteQuery.makeVariables(~id=highlightId, ()),
      ~pollInterval=isLoaded ? 0 : 500,
      ~skip=
        switch (authentication) {
        | Authenticated(_) when rehydrated => false
        | _ when !rehydrated => true
        | Loading
        | Unauthenticated => true
        },
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

  switch (query, rehydrated, authentication) {
  | (Loading, _, _)
  | (_, false, _)
  | (_, _, Loading) => <Loading />
  | (Data(data), _, Authenticated(currentUser)) =>
    switch (data##getHighlight) {
    | Some(highlight) => <Data highlight currentUser />
    | None => <Empty />
    }
  | (NoData, true, _)
  | (Error(_), true, _)
  | (_, _, Unauthenticated) => <Empty />
  };
};
