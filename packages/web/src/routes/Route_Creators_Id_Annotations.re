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

  let creatorUsername =
    switch (Routes.CreatorsIdAnnotations.params_decode(router.Next.query)) {
    | Ok(p) => Some(p.creatorUsername)
    | _ => None
    };

  let handleAnnotationIdChange = annotationId => {
    switch (creatorUsername) {
    | Some(creatorUsername) =>
      Next.Router.replaceWithAs(
        Routes.CreatorsIdAnnotationsId.staticPath,
        Routes.CreatorsIdAnnotationsId.path(
          ~creatorUsername,
          ~annotationIdComponent=
            Lib_GraphQL.Annotation.idComponent(annotationId),
        ),
      )
    | None => ()
    };
  };

  switch (authentication) {
  | Unauthenticated => <Loading />
  | Loading
  | Authenticated(_) =>
    <QueryRenderers_Notes
      annotationId=None
      onAnnotationIdChange=handleAnnotationIdChange
      authentication
      rehydrated
    />
  };
};

let page = "creators/[creatorUsername]/annotations.js";
