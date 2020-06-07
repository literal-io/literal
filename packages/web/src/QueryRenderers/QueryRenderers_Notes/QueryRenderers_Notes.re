open Styles;
open QueryRenderers_Notes_GraphQL;

module Data = {
  [@react.component]
  let make =
      (~highlights, ~initialHighlightId, ~onHighlightIdChange, ~currentUser) => {
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
        currentUser
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
               isActive={h##id === activeHighlight##id}
             />
           </ScrollSnapList.Item>
         )}
      </ScrollSnapList.Container>
    </>;
  };
};

module Onboarding = {
  [@react.component]
  let make = (~onboardingProfileFragment, ~currentUser) => {
    <Containers_Onboarding
      profileFragment=onboardingProfileFragment
      currentUser
    />;
  };
};

module Empty = {
  [@react.component]
  let make = () => React.string("Empty...");
};

[@react.component]
let make = (~highlightId, ~onHighlightIdChange, ~currentUser) => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=
        ListHighlights.Query.makeVariables(
          ~owner=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
          (),
        ),
      ListHighlights.Query.definition,
    );

  <div className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
    {switch (query) {
     | Data(data) =>
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
     | Loading => <Loading />
     | NoData
     | Error(_) => <Empty />
     }}
  </div>;
};
