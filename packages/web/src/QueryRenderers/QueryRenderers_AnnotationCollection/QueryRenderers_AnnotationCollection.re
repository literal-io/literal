open QueryRenderers_AnnotationCollection_GraphQL;

module Data = {
  [@react.component]
  let make =
      (
        ~annotationCollection,
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

    let cacheQuery =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~id=
          Lib_GraphQL.AnnotationCollection.(
            makeIdFromComponent(
              ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
              ~creatorUsername=
                currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
              (),
            )
          ),
        (),
      );
    let cache =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
        ~query=cacheQuery,
        ~client=Providers_Apollo.client,
        (),
      );

    <div
      className={Cn.fromList([
        "w-full",
        "h-full",
        "bg-black",
        "overflow-y-scroll",
      ])}>
      <Containers_AnnotationCollectionHeader
        annotationFragment={activeAnnotation##annotationCollectionHeader}
        annotationCollectionFragment=
          {annotationCollection##annotationCollectionHeader}
        currentUser
      />
      <ScrollSnapList.Container
        direction=ScrollSnapList.Horizontal
        onIdxChange=handleIdxChange
        initialIdx=activeIdx>
        {annotations->Belt.Array.map(annotation =>
           <ScrollSnapList.Item
             key={annotation##id} direction=ScrollSnapList.Horizontal>
             <Containers_AnnotationEditor_Notes
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

module Empty = {
  [@react.component]
  let make = () => React.string("Empty...");
};

[@react.component]
let make =
    (
      ~annotationCollectionId,
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
          GetAnnotationCollection.Query.makeVariables(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=annotationCollectionId,
            (),
          )
        | _ => Js.Json.null
        },
      GetAnnotationCollection.Query.definition,
    );

  switch (query, rehydrated, authentication) {
  | (Loading, _, _)
  | (_, false, _)
  | (_, _, Loading) => <Loading />
  | (Data(data), _, Authenticated(currentUser)) =>
    data##getAnnotationCollection
    ->Belt.Option.flatMap(annotationCollection => {
        annotationCollection##first
        ->Belt.Option.flatMap(annotationPage => annotationPage##items)
        ->Belt.Option.flatMap(annotationPageItemConnection =>
            annotationPageItemConnection##items
          )
        ->Belt.Option.flatMap(annotationPageItems => {
            let annotations =
              annotationPageItems->Belt.Array.keepMap(annotationPageItem =>
                annotationPageItem->Belt.Option.map(annotationPageItem =>
                  annotationPageItem##annotation
                )
              );

            if (Js.Array2.length(annotations) > 0) {
              Some(
                <Data
                  onAnnotationIdChange
                  initialAnnotationId=None
                  currentUser
                  annotationCollection
                  annotations={
                    annotations
                    |> Ramda.sortBy(a => {
                         /** FIXME: sort in query **/
                         (
                           -.
                             a##created
                             ->Belt.Option.flatMap(Js.Json.decodeString)
                             ->Belt.Option.map(created =>
                                 created->Js.Date.fromString->Js.Date.valueOf
                               )
                             ->Belt.Option.getWithDefault(0.)
                         )
                       })
                  }
                />,
              );
            } else {
              None;
            };
          })
      })
    ->Belt.Option.getWithDefault(<Empty />)
  | (NoData, true, _)
  | (Error(_), true, _)
  | (_, _, Unauthenticated) => /** FIXME: handle redirect **/ <Empty />
  };
};
