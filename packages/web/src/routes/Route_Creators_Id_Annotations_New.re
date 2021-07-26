type searchVariant =
  | SearchVariantAnnotationId(string)
  | SearchVariantFileUrl(string);

[@react.component]
let default = (~rehydrated) => {
  let router = Next.Router.useRouter();
  let searchParams =
    router.asPath
    ->Js.String2.split("?")
    ->Belt.Array.get(1)
    ->Belt.Option.getWithDefault("")
    ->Webapi.Url.URLSearchParams.make;
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

  let handleClear = () => {
    let identityId =
      switch (user) {
      | SignedInUser({identityId})
      | GuestUser({identityId}) => Some(identityId)
      | _ => None
      };

    let _ =
      identityId->Belt.Option.forEach(identityId =>
        Next.Router.replaceWithAs(
          Routes.CreatorsIdAnnotationsNew.staticPath,
          Routes.CreatorsIdAnnotationsNew.path(~identityId),
        )
      );

    ();
  };

  let searchVariant =
    switch (
      searchParams |> Webapi.Url.URLSearchParams.get("id"),
      searchParams |> Webapi.Url.URLSearchParams.get("fileUrl"),
    ) {
    | (Some(id), _) => Some(SearchVariantAnnotationId(id))
    | (_, Some(fileUrl)) => Some(SearchVariantFileUrl(fileUrl))
    | _ => None
    };

  let main =
    switch (
      user,
      Routes.CreatorsIdAnnotationsNew.params_decode(router.Next.query),
      searchVariant,
    ) {
    | (_, Ok(_), Some(SearchVariantFileUrl(fileUrl))) =>
      switch (user) {
      | Unknown
      | SignedOutPromptAuthentication
      | SignedInUserMergingIdentites(_) =>
        <QueryRenderers_NewAnnotationFromShare.Loading />
      | _ when !rehydrated => <QueryRenderers_NewAnnotationFromShare.Loading />
      | SignedInUser({identityId})
      | GuestUser({identityId}) =>
        <QueryRenderers_NewAnnotationFromShare
          fileUrl=?{Some(fileUrl)}
          identityId
        />
      }
    | (_, Ok(_), Some(SearchVariantAnnotationId(annotationIdComponent)))
        when Js.String.length(annotationIdComponent) > 0 =>
      switch (user) {
      | Unknown
      | SignedOutPromptAuthentication
      | SignedInUserMergingIdentites(_) =>
        <QueryRenderers_NewAnnotationFromShare.Loading />
      | _ when !rehydrated => <QueryRenderers_NewAnnotationFromShare.Loading />
      | SignedInUser({identityId})
      | GuestUser({identityId}) =>
        <QueryRenderers_NewAnnotationFromShare
          annotationId=?{
            Lib_GraphQL.Annotation.makeIdFromComponent(
              ~identityId,
              ~annotationIdComponent,
            )
            ->Js.Option.some
          }
          identityId
        />
      }
    | (Unknown, _, _)
    | (SignedOutPromptAuthentication, _, _)
    | (SignedInUserMergingIdentites(_), _, _) => <Loading />
    | _ when !rehydrated => <Loading />
    | (GuestUser({identityId}), _, _)
    | (SignedInUser({identityId}), _, _) =>
      <QueryRenderers_NewAnnotation identityId />
    };

  <div className={Cn.fromList(["w-full", "h-full", "overflow-y-hidden", "flex", "flex-col", "relative"])}>
    main
    <Alert urlSearchParams=searchParams onClear=handleClear />
  </div>;
};

let page = "creators/[identityId]/annotations/new.js";
