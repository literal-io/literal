open QueryRenderers_AnnotationCollection_GraphQL;

module MainLoading = {
  [@react.component]
  let make = () =>
    <TextInput_Loading className={Cn.fromList(["px-6", "pb-4", "pt-16"])} />;
};

module Loading = {
  [@react.component]
  let make = () =>
    <div
      className={Cn.fromList([
        "w-full",
        "h-full",
        "bg-black",
        "overflow-y-scroll",
      ])}>
      <Containers_AnnotationCollectionHeader
        onCollectionsButtonClick={() => ()}
      />
      <MainLoading />
    </div>;
};

module Data = {
  [@react.component]
  let make =
      (
        ~annotations,
        ~annotationId,
        ~onAnnotationIdChange,
        ~onFetchMore,
        ~identityId,
      ) => {
    let (activeIdx, setActiveIdx) =
      React.useState(() =>
        annotationId
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
            switch (LiteralModel.Annotation.decode(json)) {
            | Ok(r) => Some(r)
            | Error(e) =>
              let _ = Error.(report(DeccoDecodeError(e)));
              None;
            }
          )
        ->Belt.Option.forEach(annotation =>
            Lib_WebView_Model_Apollo.writeToCache(~annotation, ~identityId)
          );
      ();
    };

    let handleDeleteCacheAnnotation = ev => {
      let _ =
        ev
        ->Belt.Option.flatMap(ev => ev->Js.Json.decodeObject)
        ->Belt.Option.flatMap(dict => Js.Dict.get(dict, "annotation"))
        ->Belt.Option.flatMap(json =>
            switch (LiteralModel.Annotation.decode(json)) {
            | Ok(r) => Some(r)
            | Error(_) => None
            }
          )
        ->Belt.Option.forEach(annotation =>
            Lib_WebView_Model_Apollo.deleteFromCache(~annotation, ~identityId)
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
              switch (LiteralModel.Annotation.decode(json)) {
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
          Lib_WebView_Model_Apollo.addManyToCache(~annotations, ~identityId);

        Routes.CreatorsIdAnnotationCollectionsId.(
          Next.Router.replaceWithAs(
            staticPath,
            path(
              ~identityId,
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

    <ScrollSnapList.Container
      direction=ScrollSnapList.Horizontal
      onIdxChange=handleIdxChange
      initialIdx=activeIdx
      className={Cn.fromList(["pt-14", "w-full", "h-full"])}>
      {annotations->Belt.Array.map(annotation =>
         <ScrollSnapList.Item
           key={annotation##id} direction=ScrollSnapList.Horizontal>
           <Containers_AnnotationEditor
             isVisible={annotation##id == activeAnnotation##id}
             annotationFragment={annotation##editorAnnotationFragment}
             identityId
           />
         </ScrollSnapList.Item>
       )}
    </ScrollSnapList.Container>;
  };
};

module Error = {
  [@react.component]
  let make = (~error=?) => {
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
  let make = () =>
    <div
      className={Cn.fromList([
        "w-full",
        "h-full",
        "bg-black",
        "flex",
        "flex-col",
      ])}>
      <div
        className={Cn.fromList([
          "flex",
          "flex-col",
          "justify-center",
          "items-center",
          "flex-1",
        ])}>
        <Svg
          className={Cn.fromList([
            "w-24",
            "h-24",
            "pointer-events-none",
            "text-lightDisabled",
            "opacity-50",
          ])}
          icon=Svg.waves
        />
      </div>
      <BottomAlert
        text="You have no annotations. To get started, tap the \"+\" icon in the upper right corner."
      />
    </div>;
};

[@react.component]
let make =
    (
      ~annotationCollectionIdComponent,
      ~onOpenCollectionsDrawer,
      ~onAnnotationIdChange,
      ~user: Providers_Authentication_User.t,
      ~annotationId,
      ~rehydrated,
    ) => {
  let identityId =
    switch (user) {
    | GuestUser({identityId})
    | SignedInUser({identityId}) => Some(identityId)
    | _ => None
    };
  let (_, query) =
    ApolloHooks.useQuery(
      ~skip=
        switch (user) {
        | GuestUser(_) when rehydrated => false
        | SignedInUser(_) when rehydrated => false
        | Unknown
        | SignedOutPromptAuthentication
        | _ => true
        },
      ~variables=
        switch (identityId) {
        | Some(identityId) when rehydrated =>
          GetAnnotationCollection.Query.makeVariables(
            ~creatorUsername=identityId,
            ~id=
              Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
                ~identityId,
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
      switch (nextToken, identityId) {
      | (Some(nextToken), Some(identityId)) when !query.loading =>
        let variables =
          GetAnnotationCollection.Query.makeVariables(
            ~creatorUsername=identityId,
            ~id=
              Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
                ~identityId,
                ~annotationCollectionIdComponent,
                (),
              ),
            ~nextToken=?
              Js.String2.length(nextToken) > 0 ? Some(nextToken) : None,
            (),
          );
        let updateQuery = (prev, next: ApolloHooksQuery.updateQueryOptions) => {
          let prevAnnotationPageItems =
            GetAnnotationCollection.Cache.unsafeToCache(prev)##getAnnotationCollection
            ->Js.Null.toOption
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.getWithDefault([||]);
          let (newAnnotationNextToken, nextAnnotationPageItems) =
            next
            ->ApolloHooksQuery.fetchMoreResultGet
            ->Belt.Option.flatMap(d =>
                GetAnnotationCollection.Cache.unsafeToCache(d)##getAnnotationCollection
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
          ->GetAnnotationCollection.Cache.unsafeToCache
          ->GetAnnotationCollection.setAnnotationPageItems(
              newAnnotationPageItems,
            )
          ->GetAnnotationCollection.setNextToken(newAnnotationNextToken)
          ->GetAnnotationCollection.Cache.unsafeCacheToJson;
        };
        let _ = query.fetchMore(~variables, ~updateQuery, ());
        ();
      | _ => ()
      };
    ();
  };

  let isRecentAnnotationCollection =
    annotationCollectionIdComponent
    == Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent;

  let agent =
    switch (query) {
    | {data: Some(data)} => data##getAgent
    | _ => None
    };
  let annotationCollection =
    switch (query) {
    | {data: Some(data), loading} =>
      switch (data##getAnnotationCollection) {
      | Some(annotationCollection)
          when
            loading
            && Lib_GraphQL.AnnotationCollection.idComponent(
                 annotationCollection##id,
               )
            != annotationCollectionIdComponent =>
        None
      | _ => data##getAnnotationCollection
      }
    | _ => None
    };
  let annotations =
    annotationCollection->Belt.Option.flatMap(annotationCollection =>
      annotationCollection##first
      ->Belt.Option.flatMap(annotationPage => annotationPage##items)
      ->Belt.Option.flatMap(annotationPageItemConnection =>
          annotationPageItemConnection##items
        )
      ->Belt.Option.map(annotationPageItems => {
          annotationPageItems->Belt.Array.keepMap(annotationPageItem =>
            annotationPageItem->Belt.Option.map(annotationPageItem =>
              annotationPageItem##annotation
            )
          )
        })
    );
  let activeAnnotation =
    switch (annotations, annotationId) {
    | (Some(annotations), None) => Belt.Array.get(annotations, 0)
    | (Some(annotations), Some(annotationId)) =>
      annotations->Belt.Array.getBy(annotation =>
        annotation##id == annotationId
      )
    | _ => None
    };
  let isLoading =
    switch (annotationCollection, user) {
    | _ when !rehydrated => true
    | (None, _) when query.loading => true
    | (_, Unknown)
    | (_, SignedInUserMergingIdentites(_)) => true
    | (Some(annotationCollection), _)
        when
          query.loading
          && Lib_GraphQL.AnnotationCollection.idComponent(
               annotationCollection##id,
             )
          != annotationCollectionIdComponent =>
      true
    | _ => false
    };
  let isEmpty =
    switch (annotations) {
    | None when !isLoading => true
    | Some(annotations) when Js.Array2.length(annotations) == 0 => true
    | _ => false
    };
  let isOnboarding =
    switch (agent) {
    | None when !isLoading && isEmpty && isRecentAnnotationCollection => true
    | _ => false
    };

  let header = {
    let annotationCollectionFragment =
      switch (annotationCollection, identityId) {
      | (Some(annotationCollection), _) =>
        Some(annotationCollection##annotationCollectionHeader)
      | (_, Some(identityId)) =>
        Containers_AnnotationCollectionHeader.fragmentFromCache(
          ~annotationCollectionIdComponent,
          ~identityId,
          (),
        )
      | _ =>
        Containers_AnnotationCollectionHeader.fragmentFromCache(
          ~annotationCollectionIdComponent,
          (),
        )
      };
    let annotationFragment =
      switch (activeAnnotation) {
      | Some(activeAnnotation) =>
        Some(activeAnnotation##annotationCollectionHeader)
      | None => None
      };

    <Containers_AnnotationCollectionHeader
      ?annotationCollectionFragment
      ?annotationFragment
      ?identityId
      hideDelete=isEmpty
      onCollectionsButtonClick=onOpenCollectionsDrawer
    />;
  };

  let main =
    switch (identityId, annotations, user) {
    | _ when isLoading => <MainLoading />
    | (Some(_), _, _) when isOnboarding =>
      <Containers_Onboarding user> <MainLoading /> </Containers_Onboarding>
    | _ when isEmpty && isRecentAnnotationCollection => <Empty />
    | (Some(identityId), _, _) when isEmpty =>
      <Redirect
        staticPath=Routes.CreatorsIdAnnotationCollectionsId.staticPath
        path={Routes.CreatorsIdAnnotationCollectionsId.path(
          ~identityId,
          ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
        )}>
        <MainLoading />
      </Redirect>
    | (Some(identityId), Some(annotations), _) =>
      <Data
        onAnnotationIdChange
        onFetchMore=handleFetchMore
        annotationId
        identityId
        annotations
      />
    | (_, _, SignedOutPromptAuthentication) =>
      <Redirect
        staticPath={Routes.Authenticate.path()}
        path={Routes.Authenticate.path()}>
        <MainLoading />
      </Redirect>
    | _ =>
      switch (query.error) {
      | Some(error) => <Error error />
      | _ => <Error />
      }
    };

  <> header main </>;
};
