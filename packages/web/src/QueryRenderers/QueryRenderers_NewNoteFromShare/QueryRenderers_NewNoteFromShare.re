open Styles;
open QueryRenderers_NewNoteFromShare_GraphQL;

let pollInterval = 500;
let pollTimeout = 15 * 1000;
let noDataAlert = "Unable to parse image. Make sure the text is clearly highlighted and try again, or enter the text manually.";

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
          size={MaterialUi.CircularProgress.Size.int(26)}
          classes={MaterialUi.CircularProgress.Classes.make(
            ~colorPrimary=cn(["text-black"]),
            (),
          )}
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
      ~pollInterval=isLoaded ? 0 : pollInterval,
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
    React.useEffect0(() => {
      let timeoutId =
        Js.Global.setTimeout(() => setIsLoaded(_ => true), pollTimeout);
      Some(() => {Js.Global.clearTimeout(timeoutId)});
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (query) {
          | Data(data) =>
            switch (data##getHighlight) {
            | Some(_) => setIsLoaded(_ => true)
            | _ => ()
            }
          | _ => ()
          };
        None;
      },
      [|query|],
    );

  switch (query, rehydrated, authentication, isLoaded) {
  | (Loading, _, _, _)
  | (_, false, _, _)
  | (_, _, Loading, _)
  | (_, _, _, false) => <Loading />
  | (Data(data), _, Authenticated(currentUser), true) =>
    switch (data##getHighlight) {
    | Some(highlight) => <Data highlight currentUser />
    | None =>
      <Redirect
        path="/notes/new"
        query={Raw.merge(
          Alert.(query_encode({alert: noDataAlert})),
          Routes.New.params_encode({
            id: None,
            initialPhaseState: Some(`PhaseTextInput),
          }),
        )}
      />
    }
  | (NoData, true, _, _)
  | (Error(_), true, _, _) =>
    <Redirect
      path="/notes/new"
      query={Raw.merge(
        Alert.(query_encode({alert: noDataAlert})),
        Routes.New.params_encode({
          id: None,
          initialPhaseState: Some(`PhaseTextInput),
        }),
      )}
    />
  | (_, _, Unauthenticated, _) => <Loading />
  };
};
