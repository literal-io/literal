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

  Js.log3(
    router.Next.query,
    authentication,
    Routes.CreatorsIdAnnotationsNew.queryParams_decode(router.Next.query),
  );

  <>
    {switch (
       authentication,
       Routes.CreatorsIdAnnotationsNew.queryParams_decode(router.Next.query),
     ) {
     | (Unauthenticated, _) => 
      Js.log("loading 1");
      <Loading />
     | (_, Ok({id: Some(annotationIdComponent), creatorUsername}))
         when Js.String.length(annotationIdComponent) > 0 =>
        Js.log("new annotation from share");
       <QueryRenderers_NewAnnotationFromShare
         annotationId={Lib_GraphQL.Annotation.makeIdFromComponent(
           ~creatorUsername,
           ~annotationIdComponent,
         )}
         authentication
         rehydrated
       />
     | (Loading, _) => 
        Js.log("loading 2");
       <Loading />
     | _ when !rehydrated =>
        Js.log("loading 3");
        <Loading />
     | (Authenticated(currentUser), _) =>
        Js.log("new annotation");
       <QueryRenderers_NewAnnotation currentUser />
     }}
    <Alert query={router.query} />
  </>;
};

let page = "creators/[creatorUsername]/annotations/new.js";
