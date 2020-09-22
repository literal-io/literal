[@react.component]
let default = (~rehydrated) => {
  let router = Next.Router.useRouter();
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

  let handleAnnotationIdChange = _ =>
    /** TODO: implement **/
    {
      ();
    };

  switch (
    authentication,
    Routes.CreatorsIdAnnotationCollectionsId.params_decode(router.Next.query),
  ) {
  | (Unauthenticated, Ok(_)) => <Loading />
  | (Loading, Ok(routeParams))
  | (Authenticated(_), Ok(routeParams)) =>
    <QueryRenderers_AnnotationCollection
      annotationCollectionIdComponent={routeParams.annotationCollectionIdComponent}
      onAnnotationIdChange=handleAnnotationIdChange
      authentication
      rehydrated
    />
  | (_, Error(_)) =>
    /** FIXME: redirect to /creators/[id]/annotation-collections **/
    <Loading />
  };
};

let page = "creators/[creatorUsername]/annotation-collections/[annotationCollectionIdComponent].js";
