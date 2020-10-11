open QueryRenderers_AnnotationCollection_GraphQL;

module Data = {
  [@react.component]
  let make =
      (
        ~annotationCollection,
        ~annotations,
        ~initialAnnotationId,
        ~onAnnotationIdChange,
        ~onFetchMore,
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
          if (activeIdx + 25 > Js.Array2.length(annotations)) {
            let _ = onFetchMore();
            ();
          };
          None;
        },
        [|activeAnnotation|],
      );

    let handleIdxChange = idx => {
      setActiveIdx(_ => idx);
    };

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
             <Containers_AnnotationEditor
               annotationFragment={annotation##editorAnnotationFragment}
               currentUser
             />
           </ScrollSnapList.Item>
         )}
      </ScrollSnapList.Container>
      <Containers_NewTagInput
        currentUser
        annotationFragment={
          annotations
          ->Belt.Array.get(activeIdx)
          ->Belt.Option.getExn
          ->(a => a##newTagInputFragment)
        }
      />
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
      ~annotationCollectionIdComponent,
      ~onAnnotationIdChange,
      ~authentication: Hooks_CurrentUserInfo_Types.state,
      ~initialAnnotationId,
      ~rehydrated,
    ) => {
  let (_, query) =
    ApolloHooks.useQuery(
      ~skip=
        switch (authentication) {
        | Authenticated(_) when rehydrated => false
        | Loading
        | Unauthenticated => true
        | _ => true
        },
      ~variables=
        switch (authentication) {
        | Authenticated(currentUser) when rehydrated =>
          GetAnnotationCollection.Query.makeVariables(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=
              Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
                ~creatorUsername=
                  currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                ~annotationCollectionIdComponent,
                (),
              ),
            (),
          )
        | _ => Js.Json.null
        },
      GetAnnotationCollection.Query.definition,
    );

  let handleFetchMore = () => {
    let nextToken =
      query.data
      ->Belt.Option.flatMap(d => d##getAnnotationCollection)
      ->Belt.Option.flatMap(d => d##first)
      ->Belt.Option.flatMap(d => d##items)
      ->Belt.Option.flatMap(d => d##nextToken);

    let _ =
      switch (nextToken, authentication) {
      | (Some(nextToken), Authenticated(currentUser)) when !query.loading =>
        let variables =
          GetAnnotationCollection.Query.makeVariables(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=
              Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
                ~creatorUsername=
                  currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                ~annotationCollectionIdComponent,
                (),
              ),
            ~nextToken=?
              Js.String2.length(nextToken) > 0 ? Some(nextToken) : None,
            (),
          );
        let updateQuery = (prev, next: ApolloHooksQuery.updateQueryOptions) => {
          let prevAnnotationPageItems =
            GetAnnotationCollection.unsafeToCache(prev)##getAnnotationCollection
            ->Js.Null.toOption
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.getWithDefault([||]);
          let (newAnnotationNextToken, nextAnnotationPageItems) =
            next
            ->ApolloHooksQuery.fetchMoreResultGet
            ->Belt.Option.flatMap(d =>
                GetAnnotationCollection.unsafeToCache(d)##getAnnotationCollection
                ->Js.Null.toOption
              )
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.map(d =>
                (
                  d##nextToken,
                  d##items->Js.Null.toOption->Belt.Option.getWithDefault([||]),
                )
              )
            ->Belt.Option.getWithDefault((Js.Null.empty, [||]));
          let newAnnotationPageItems =
            Belt.Array.concat(
              prevAnnotationPageItems,
              nextAnnotationPageItems,
            )
            ->Js.Null.return;

          prev
          ->GetAnnotationCollection.unsafeToCache
          ->GetAnnotationCollection.setAnnotationPageItems(
              newAnnotationPageItems,
            )
          ->GetAnnotationCollection.setNextToken(newAnnotationNextToken)
          ->GetAnnotationCollection.unsafeCacheToJson;
        };
        let _ = query.fetchMore(~variables, ~updateQuery, ());
        ();
      | _ => ()
      };
    ();
  };

  switch (query, rehydrated, authentication) {
  | (_, false, _)
  | (_, _, Loading)
  | ({data: None, loading: true}, _, _) => <Loading />
  | ({data: Some(data), loading}, true, Authenticated(currentUser)) =>
    let isRecentAnnotationCollection =
      annotationCollectionIdComponent
      == Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent;
    switch (
      data##getAnnotationCollection
      ->Belt.Option.flatMap(annotationCollection =>
          annotationCollection##first
          ->Belt.Option.flatMap(annotationPage => annotationPage##items)
          ->Belt.Option.flatMap(annotationPageItemConnection =>
              annotationPageItemConnection##items
            )
          ->Belt.Option.map(annotationPageItems => {
              let annotations =
                annotationPageItems->Belt.Array.keepMap(annotationPageItem =>
                  annotationPageItem->Belt.Option.map(annotationPageItem =>
                    annotationPageItem##annotation
                  )
                );
              (annotationCollection, annotations);
            })
        )
    ) {
    | None when loading => <Loading />
    | None when isRecentAnnotationCollection =>
      <Containers_Onboarding currentUser onAnnotationIdChange />;
    | None =>
      <Redirect
        staticPath=Routes.CreatorsIdAnnotationCollectionsId.staticPath
        path={Routes.CreatorsIdAnnotationCollectionsId.path(
          ~creatorUsername=currentUser.username,
          ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
        )}>
        <Loading />
      </Redirect>
    | Some((_, annotations))
        when
          isRecentAnnotationCollection && Js.Array2.length(annotations) == 0 =>
      <Containers_Onboarding currentUser onAnnotationIdChange />;
    | Some((_, annotations)) when Js.Array2.length(annotations) == 0 =>
      <Redirect
        staticPath=Routes.CreatorsIdAnnotationCollectionsId.staticPath
        path={Routes.CreatorsIdAnnotationCollectionsId.path(
          ~creatorUsername=currentUser.username,
          ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
        )}>
        <Loading />
      </Redirect>
    | Some((annotationCollection, annotations)) =>
      <Data
        onAnnotationIdChange
        onFetchMore=handleFetchMore
        initialAnnotationId
        currentUser
        annotationCollection
        annotations
      />
    };
  | ({error: Some(_error)}, _, _) =>
    /** FIXME: handle apollo error **/ <Empty />
  | ({error: None, data: None, loading: false}, _, _) =>
    /** FIXME: handle unexpected error **/ <Empty />
  | (_, _, Unauthenticated) =>
    <Redirect
      staticPath={Routes.Authenticate.path()}
      path={Routes.Authenticate.path()}>
      <Loading />
    </Redirect>
  };
};
