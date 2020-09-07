open Styles;
open QueryRenderers_Notes_GraphQL;

module Data = {
  [@react.component]
  let make =
      (
        ~annotations,
        ~initialAnnotationId,
        ~onAnnotationIdChange,
        ~currentUser,
      ) => {
    let (activeIdx, setActiveIdx) =
      React.useState(() =>
        initialAnnotationId
        ->Belt.Option.flatMap(initialAnnotationId =>
            annotations->Belt.Array.getIndexBy(a =>
              a##id === initialAnnotationId
            )
          )
        ->Belt.Option.getWithDefault(0)
      );

    let activeAnnotation = annotations->Belt.Array.getExn(activeIdx);

    let _ =
      React.useEffect1(
        () => {
          let _ = onAnnotationIdChange(activeAnnotation##id);
          None;
        },
        [|activeAnnotation|],
      );

    let handleIdxChange = idx => setActiveIdx(_ => idx);

    <div
      className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
      <Containers_NoteHeader
        annotationFragment={activeAnnotation##headerAnnotationFragment}
        currentUser
      />
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal
        onIdxChange=handleIdxChange
        initialIdx=activeIdx>
        {annotations->Belt.Array.map(annotation =>
           <ScrollSnapList.Item
             key={annotation##id} direction=ScrollSnapList.Horizontal>
             <Containers_NoteEditor_Notes
               annotationFragment={annotation##editorAnnotationFragment}
               isActive={annotation##id === activeAnnotation##id}
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
  let make = (~currentUser) => {
    <div
      className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
      <Containers_Onboarding currentUser />
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
      ~annotationId,
      ~onAnnotationIdChange,
      ~authentication: Hooks_CurrentUserInfo_Types.state,
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
          ListAnnotations.Query.makeVariables(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            (),
          )
        | _ => Js.Json.null
        },
      ListAnnotations.Query.definition,
    );

  switch (query, rehydrated, authentication) {
  | (Loading, _, _)
  | (_, false, _)
  | (_, _, Loading) => <Loading />
  | (Data(data), _, Authenticated(currentUser)) =>
    switch (data##listAnnotations->Belt.Option.flatMap(h => h##items)) {
    | Some(annotations) when Array.length(annotations) > 0 =>
      <Data
        onAnnotationIdChange
        initialAnnotationId=annotationId
        currentUser
        annotations={
          annotations->Belt.Array.keepMap(i => i)
          |> Ramda.sortBy(a => {
               -.
                 a##created
                 ->Belt.Option.flatMap(Js.Json.decodeString)
                 ->Belt.Option.map(created =>
                     created->Js.Date.fromString->Js.Date.valueOf
                   )
                 ->Belt.Option.getWithDefault(0.)
             })
        }
      />
    | Some(_) /*** empty arr **/
    | None => <Onboarding currentUser />
    }
  | (NoData, true, _)
  | (Error(_), true, _)
  | (_, _, Unauthenticated) => <Empty />
  };
};
