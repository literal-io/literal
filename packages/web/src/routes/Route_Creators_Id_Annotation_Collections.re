[@react.component]
let default = (~rehydrated) => {
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


  <QueryRenderers_AnnotationCollections rehydrated user />;
};

let page = "creators/[identityId]/annotation-collections.js";
