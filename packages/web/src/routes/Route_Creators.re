[@react.component]
let default = (~rehydrated) => {
  let Providers_Authentication.{user} =
    React.useContext(Providers_Authentication.authenticationContext);

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (user) {
          | SignedInUser({identityId})
          | GuestUser({identityId}) =>
            Routes.CreatorsIdAnnotationCollectionsId.(
              Next.Router.replaceWithAs(
                staticPath,
                path(
                  ~identityId,
                  ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                ),
              )
            )
          | SignedOutPromptAuthentication =>
            Next.Router.replace(Routes.Authenticate.path())
          | Unknown => ()
          };

        None;
      },
      [|user|],
    );

  <QueryRenderers_AnnotationCollection.Loading />;
};

let page = "creators.js"
