[@react.component]
let default = (~rehydrated) => {
  let router = Next.Router.useRouter();
  let searchParams =
    Hooks_SearchParams.use(
      Routes.CreatorsIdAnnotationCollectionsId.parseSearch,
    );
  let authentication = Hooks_CurrentUserInfo.use();
  let (isCollectionsDrawerVisible, setIsCollectionsDrawerVisible) =
    React.useState(_ => false);

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (authentication) {
          | Unauthenticated => Next.Router.replace(Routes.Authenticate.path())
          | _ => ()
          };
        None;
      },
      [|authentication|],
    );

  let handleAnnotationIdChange = annotationId => {
    let _ =
      switch (
        Routes.CreatorsIdAnnotationCollectionsId.params_decode(
          router.Next.query,
        )
      ) {
      | Ok(routeParams) =>
        let search =
          Routes.CreatorsIdAnnotationCollectionsId.(
            makeSearch({
              ...searchParams,
              annotationId:
                annotationId
                ->Lib_GraphQL.Annotation.idComponent
                ->Js.Option.some,
            })
          );
        let path =
          Routes.CreatorsIdAnnotationCollectionsId.path(
            ~creatorUsername=routeParams.creatorUsername,
            ~annotationCollectionIdComponent=
              routeParams.annotationCollectionIdComponent,
          );

        Next.Router.(
          replaceWithOptions(
            Routes.CreatorsIdAnnotationCollectionsId.staticPath,
            Js.String2.length(search) > 0 ? path ++ "?" ++ search : path,
            {shallow: true},
          )
        );
      | Error(_) => ()
      };
    ();
  };

  switch (
    authentication,
    Routes.CreatorsIdAnnotationCollectionsId.params_decode(router.Next.query),
  ) {
  | (Loading, Ok(routeParams))
  | (Authenticated(_), Ok(routeParams)) =>
    <>
      <QueryRenderers_AnnotationCollectionsDrawer
        onClose={() => setIsCollectionsDrawerVisible(_ => false)}
        isVisible=isCollectionsDrawerVisible
        authentication
        rehydrated
      />
      <QueryRenderers_AnnotationCollection
        annotationCollectionIdComponent={
                                          routeParams.
                                            annotationCollectionIdComponent
                                        }
        onOpenCollectionsDrawer={() =>
          setIsCollectionsDrawerVisible(_ => true)
        }
        onAnnotationIdChange=handleAnnotationIdChange
        annotationId={
          searchParams.annotationId
          ->Belt.Option.map(annotationIdComponent =>
              Lib_GraphQL.Annotation.makeIdFromComponent(
                ~annotationIdComponent,
                ~creatorUsername=routeParams.creatorUsername,
              )
            )
        }
        authentication
        rehydrated
      />
    </>
  | (Loading, Error(_)) => <Loading />
  | (Authenticated(currentUser), Error(_)) =>
    <Redirect
      staticPath=Routes.CreatorsIdAnnotationCollectionsId.staticPath
      path={Routes.CreatorsIdAnnotationCollectionsId.path(
        ~creatorUsername=currentUser.username,
        ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
      )}>
      <Loading />
    </Redirect>
  | (Unauthenticated, Ok(_))
  | (Unauthenticated, Error(_)) =>
    <Redirect
      staticPath={Routes.Authenticate.path()}
      path={Routes.Authenticate.path()}>
      <Loading />
    </Redirect>
  };
};

let page = "creators/[creatorUsername]/annotation-collections/[annotationCollectionIdComponent].js";
