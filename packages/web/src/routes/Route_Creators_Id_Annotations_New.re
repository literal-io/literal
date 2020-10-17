[@react.component]
let default = (~rehydrated) => {
  let router = Next.Router.useRouter();
  let searchParams =
    router.asPath
    ->Js.String2.split("?")
    ->Belt.Array.get(1)
    ->Belt.Option.getWithDefault("")
    ->Webapi.Url.URLSearchParams.make;
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

  let handleClear = () =>
    switch (authentication) {
    | Authenticated(currentUser) =>
      Next.Router.replaceWithAs(
        Routes.CreatorsIdAnnotationsNew.staticPath,
        Routes.CreatorsIdAnnotationsNew.path(
          ~creatorUsername=
            currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ),
      )
    | _ => ()
    };

  <>
    {switch (
       authentication,
       Routes.CreatorsIdAnnotationsNew.params_decode(router.Next.query),
       searchParams |> Webapi.Url.URLSearchParams.get("id"),
       searchParams |> Webapi.Url.URLSearchParams.get("fileUrl"),
     ) {
     | (Unauthenticated, _, _, _) => <Loading />
     | (_, Ok(_), _, Some(fileUrl)) =>
       <QueryRenderers_NewAnnotationFromShare
         fileUrl=?{Some(fileUrl)}
         authentication
         rehydrated
       />
     | (_, Ok({creatorUsername}), Some(annotationIdComponent), _)
         when Js.String.length(annotationIdComponent) > 0 =>
       <QueryRenderers_NewAnnotationFromShare
         annotationId=?{
           Some(
             Lib_GraphQL.Annotation.makeIdFromComponent(
               ~creatorUsername,
               ~annotationIdComponent,
             ),
           )
         }
         authentication
         rehydrated
       />
     | (Loading, _, _, _) => <Loading />
     | _ when !rehydrated => <Loading />
     | (Authenticated(currentUser), _, _, _) =>
       <QueryRenderers_NewAnnotation currentUser />
     }}
    <Alert urlSearchParams=searchParams onClear=handleClear />
  </>;
};

let page = "creators/[creatorUsername]/annotations/new.js";
