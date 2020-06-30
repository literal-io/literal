open Styles;
open QueryRenderers_Notes_GraphQL;

module Data = {
  [@react.component]
  let make =
      (~highlights, ~initialHighlightId, ~onHighlightIdChange, ~currentUser) => {
    let (activeIdx, setActiveIdx) =
      React.useState(() =>
        initialHighlightId
        ->Belt.Option.flatMap(initialHighlightId =>
            highlights->Belt.Array.getIndexBy(h =>
              h##id === initialHighlightId
            )
          )
        ->Belt.Option.getWithDefault(0)
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

    <div
      className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
      <Containers_NoteHeader
        highlightFragment={activeHighlight##headerHighlightFragment}
        currentUser
      />
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal
        onIdxChange=handleIdxChange
        initialIdx=activeIdx>
        {highlights->Belt.Array.map(h =>
           <ScrollSnapList.Item
             key={h##id} direction=ScrollSnapList.Horizontal>
             <Containers_NoteEditor_Notes
               highlightFragment={h##editorHighlightFragment}
               isActive={h##id === activeHighlight##id}
               currentUser
             />
           </ScrollSnapList.Item>
         )}
      </ScrollSnapList.Container>
    </div>;
  };
};

module Onboarding = {
  [@react.component]
  let make = (~onboardingProfileFragment, ~currentUser) => {
    <div
      className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
      <Containers_Onboarding
        profileFragment=onboardingProfileFragment
        currentUser
      />
    </div>;
  };
};

module Empty = {
  [@react.component]
  let make = () => React.string("Empty...");
};

module Loading = {
  [@react.component]
  let make = () => {
    <>
      <Containers_NoteHeader />
      <TextInput_Loading className={cn(["px-6", "pb-4", "pt-16"])} />
    </>;
  };
};

[@react.component]
let make =
    (
      ~highlightId,
      ~onHighlightIdChange,
      ~authentication: CurrentUserInfo.state,
      ~rehydrated,
    ) => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~skip=
        switch (authentication) {
        | Authenticated(_) when rehydrated => false
        | _ when !rehydrated => true
        | Loading
        | Unauthenticated => true
        },
      ~variables=
        switch (authentication) {
        | Authenticated(currentUser) when rehydrated =>
          ListHighlights.Query.makeVariables(
            ~owner=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            (),
          )
        | _ => Js.Json.null
        },
      ListHighlights.Query.definition,
    );

  switch (query, rehydrated, authentication) {
  | (Loading, _, _)
  | (_, false, _)
  | (_, _, Loading) => <Loading />
  | (Data(data), _, Authenticated(currentUser)) =>
    switch (
      data##listHighlights->Belt.Option.flatMap(h => h##items),
      data##getProfile,
    ) {
    | (Some(highlights), _) when Array.length(highlights) > 0 =>
      <Data
        onHighlightIdChange
        initialHighlightId=highlightId
        currentUser
        highlights={
          highlights->Belt.Array.keepMap(i => i)
          |> Ramda.sortBy(h => {
               -. h##createdAt->Js.Date.fromString->Js.Date.valueOf
             })
        }
      />
    | (_, Some(profile)) when !profile##isOnboarded =>
      <Onboarding
        onboardingProfileFragment={profile##onboardingProfileFragment}
        currentUser
      />
    | _ => <Empty />
    }
  | (NoData, true, _)
  | (Error(_), true, _)
  | (_, _, Unauthenticated) => <Empty />
  };
};
