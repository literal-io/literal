[@react.component]
let default = (~rehydrated) => {
  let router = Next.Router.useRouter();
  let searchParams =
    router.asPath
    ->Js.String2.split("?")
    ->Belt.Array.get(1)
    ->Belt.Option.getWithDefault("")
    ->Webapi.Url.URLSearchParams.make
    ->Routes.CreatorsIdAnnotationCollectionsId.parseSearch;
  let authentication = Hooks_CurrentUserInfo.use();

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
        Next.Router.replaceWithAs(
          Routes.CreatorsIdAnnotationCollectionsId.staticPath,
          Js.String2.length(search) > 0 ? path ++ "?" ++ search : path,
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
    <QueryRenderers_AnnotationCollection
      annotationCollectionIdComponent={
                                        routeParams.
                                          annotationCollectionIdComponent
                                      }
      onAnnotationIdChange=handleAnnotationIdChange
      initialAnnotationId={
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
