[@react.component]
let default = (~rehydrated) => {
  let router = Next.Router.useRouter();
  let searchParams =
    Hooks_SearchParams.use(
      Routes.CreatorsIdAnnotationCollectionsId.parseSearch,
    );
  let Providers_Authentication.{user} =
    React.useContext(Providers_Authentication.authenticationContext);

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (user) {
          | SignedOutPromptAuthentication =>
            Next.Router.replace(Routes.Authenticate.path())
          | _ => ()
          };
        None;
      },
      [|user|],
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
            ~identityId=routeParams.identityId,
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
    user,
    Routes.CreatorsIdAnnotationCollectionsId.params_decode(router.Next.query),
  ) {
  | (Unknown, Ok(routeParams))
  | (SignedInUser(_), Ok(routeParams))
  | (GuestUser(_), Ok(routeParams))
  | (SignedInUserMergingIdentites(_), Ok(routeParams)) =>
    <QueryRenderers_AnnotationCollection
      annotationCollectionIdComponent={
                                        routeParams.
                                          annotationCollectionIdComponent
                                      }
      onAnnotationIdChange=handleAnnotationIdChange
      annotationId={
        searchParams.annotationId
        ->Belt.Option.map(annotationIdComponent =>
            Lib_GraphQL.Annotation.makeIdFromComponent(
              ~annotationIdComponent,
              ~identityId=routeParams.identityId,
            )
          )
      }
      user
      rehydrated
    />
  | (Unknown, Error(_)) => <Loading />
  | (GuestUser({identityId}), Error(_))
  | (SignedInUser({identityId}), Error(_))
  | (SignedInUserMergingIdentites({identityId}), Error(_)) =>
    <Redirect
      staticPath=Routes.CreatorsIdAnnotationCollectionsId.staticPath
      path={Routes.CreatorsIdAnnotationCollectionsId.path(
        ~identityId,
        ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
      )}>
      <QueryRenderers_AnnotationCollection.Loading />
    </Redirect>
  | (SignedOutPromptAuthentication, _) =>
    <Redirect
      staticPath={Routes.Authenticate.path()}
      path={Routes.Authenticate.path()}>
      <QueryRenderers_AnnotationCollection.Loading />
    </Redirect>
  };
};

let page = "creators/[identityId]/annotation-collections/[annotationCollectionIdComponent].js";
