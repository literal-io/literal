open QueryRenderers_AnnotationCollection_GraphQL;

module Loading = {
  [@react.component]
  let make = () => {
    <div className={Cn.fromList(["w-full", "h-full", "bg-black"])}>
      <Containers_AnnotationCollectionHeader />
      <TextInput_Loading className={Cn.fromList(["px-6", "pb-4", "pt-16"])} />
      <Containers_NewTagInput />
    </div>;
  };
};

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

    let activeAnnotation =
      annotations
      ->Belt.Array.get(activeIdx)
      ->Belt.Option.getWithDefault(
          Belt.Array.getExn(annotations, Js.Array2.length(annotations) - 1),
        );

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

    let handleSetCacheAnnotation = ev => {
      let _ =
        ev
        ->Belt.Option.flatMap(Js.Json.decodeObject)
        ->Belt.Option.flatMap(dict => Js.Dict.get(dict, "annotation"))
        ->Belt.Option.flatMap(json =>
            switch (Lib_WebView_Model_Annotation.decode(json)) {
            | Ok(r) => Some(r)
            | Error(e) =>
              let _ = Error.(report(DeccoDecodeError(e)));
              None;
            }
          )
        ->Belt.Option.forEach(annotation =>
            Lib_WebView_Model_Apollo.writeToCache(~annotation, ~currentUser)
          );
      ();
    };

    let handleDeleteCacheAnnotation = ev => {
      let _ =
        ev
        ->Belt.Option.flatMap(ev => ev->Js.Json.decodeObject)
        ->Belt.Option.flatMap(dict => Js.Dict.get(dict, "annotation"))
        ->Belt.Option.flatMap(json =>
            switch (Lib_WebView_Model_Annotation.decode(json)) {
            | Ok(r) => Some(r)
            | Error(e) => None
            }
          )
        ->Belt.Option.forEach(annotation =>
            Lib_WebView_Model_Apollo.deleteFromCache(
              ~annotation,
              ~currentUser,
            )
          );
      ();
    };

    let handleAddCacheAnnotations = ev => {
      let annotations =
        ev
        ->Belt.Option.flatMap(Js.Json.decodeObject)
        ->Belt.Option.flatMap(d => d->Js.Dict.get("annotations"))
        ->Belt.Option.flatMap(Js.Json.decodeString)
        ->Belt.Option.map(a =>
            try(a->Js.Json.parseExn) {
            | _ => Js.Json.null
            }
          )
        ->Belt.Option.flatMap(Js.Json.decodeArray)
        ->Belt.Option.map(json => {
            json->Belt.Array.keepMap(json =>
              switch (Lib_WebView_Model_Annotation.decode(json)) {
              | Ok(r) => Some(r)
              | Error(e) =>
                Js.log2("Error decoding annotation", e);
                None;
              }
            )
          })
        ->Belt.Option.getWithDefault([||]);

      if (Js.Array2.length(annotations) > 0) {
        let _ =
          Lib_WebView_Model_Apollo.addManyToCache(~annotations, ~currentUser);
        Routes.CreatorsIdAnnotationCollectionsId.(
          Next.Router.replaceWithAs(
            staticPath,
            path(
              ~creatorUsername=currentUser.username,
              ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
            ),
          )
        );
      };
    };

    let _ =
      React.useEffect0(() => {
        let eventHandlers = [|
          ("SET_CACHE_ANNOTATION", handleSetCacheAnnotation),
          ("DELETE_CACHE_ANNOTATION", handleDeleteCacheAnnotation),
          ("ADD_CACHE_ANNOTATIONS", handleAddCacheAnnotations),
        |];

        let _ =
          eventHandlers->Belt.Array.forEach(Webview.WebEventHandler.register);

        Some(
          () => {
            eventHandlers->Belt.Array.forEach(((type_, _)) =>
              Webview.WebEventHandler.unregister(type_)
            )
          },
        );
      });

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
               isVisible={annotation##id == activeAnnotation##id}
               annotationFragment={annotation##editorAnnotationFragment}
               currentUser
             />
           </ScrollSnapList.Item>
         )}
      </ScrollSnapList.Container>
      <Containers_NewTagInput
        currentUser
        annotationFragment={activeAnnotation##newTagInputFragment}
      />
    </div>;
  };
};

module Error = {
  [@react.component]
  let make = (~error) => {
    let _ =
      React.useEffect1(
        () => {
          Js.log(error);
          None;
        },
        [|error|],
      );

    <ErrorDisplay />;
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
      <Containers_Onboarding currentUser onAnnotationIdChange />
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
      <Containers_Onboarding currentUser onAnnotationIdChange />
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
  | ({error: Some(error)}, _, _) => <Error error />
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
