[@react.component]
let default = (~rehydrated) => {
  let Providers_Authentication.{user} =
    React.useContext(Providers_Authentication.authenticationContext);
  let router = Next.Router.useRouter();

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

  switch (
    Routes.CreatorsIdAnnotationCollections.params_decode(router.Next.query)
  ) {
  | Ok(_) => <QueryRenderers_AnnotationCollections rehydrated user />
  | _ =>
    <QueryRenderers_AnnotationCollections.Loading
      activeCollectionType=QueryRenderers_AnnotationCollections.defaultCollectionType
      onActiveCollectionTypeChange={_ => ()}
    />
  };
};

let page = "creators/[identityId]/annotation-collections.js";
