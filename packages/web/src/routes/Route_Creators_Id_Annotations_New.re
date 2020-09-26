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

  <>
    {switch (
       authentication,
       Routes.CreatorsIdAnnotationsNew.queryParams_decode(router.Next.query),
     ) {
     | (Unauthenticated, _) => <Loading />
     | (_, Ok({id: Some(annotationIdComponent), creatorUsername}))
         when Js.String.length(annotationIdComponent) > 0 =>
       <QueryRenderers_NewAnnotationFromShare
         annotationId={Lib_GraphQL.Annotation.makeIdFromComponent(
           ~creatorUsername,
           ~annotationIdComponent,
         )}
         authentication
         rehydrated
       />
     | (Loading, _) => <Loading />
     | _ when !rehydrated => <Loading />
     | (Authenticated(currentUser), Ok({initialPhaseState})) =>
       <QueryRenderers_NewAnnotation currentUser ?initialPhaseState />
     | (Authenticated(currentUser), _) =>
       <QueryRenderers_NewAnnotation currentUser />
     }}
    <Alert query={router.query} />
  </>;
};

let page = "creators/[creatorUsername]/annotations/new.js";
