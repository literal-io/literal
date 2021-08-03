[@react.component]
let default = () => {
  let Providers_Authentication.{user, setUser} =
    React.useContext(Providers_Authentication.authenticationContext);
  let router = Next.Router.useRouter();
  let _ =
    React.useEffect2(
      () => {
        // When we return after authentication, ensure identity id encoded in the route is updated.
        let _ =
          switch (
            user,
            Routes.CreatorsIdSettings.params_decode(router.Next.query),
          ) {
          | (SignedInUser({identityId}), Ok({identityId: routeIdentityId}))
          | (
              SignedInUserMergingIdentites({identityId}),
              Ok({identityId: routeIdentityId}),
            )
          | (GuestUser({identityId}), Ok({identityId: routeIdentityId}))
              when identityId != routeIdentityId =>
            Next.Router.replaceWithAs(
              Routes.CreatorsIdSettings.staticPath,
              Routes.CreatorsIdSettings.path(~identityId),
            )
          | _ => ()
          };
        None;
      },
      (user, router.query),
    );
  let nativeAppVersion =
    Webview.JavascriptInterface.getVersionName()
    ->Belt.Option.flatMap(versionName =>
        versionName->Js.String2.split("-")->Belt.Array.get(0)
      );

  let viewURI = url => {
    let _ =
      Webview.(
        postMessage(
          WebEvent.make(
            ~type_="ACTION_VIEW_URI",
            ~data=
              Js.Json.object_(
                Js.Dict.fromList([("uri", Js.Json.string(url))]),
              ),
            (),
          ),
        )
      );
    ();
  };

  let handleClickChangelog = () =>
    nativeAppVersion->Belt.Option.forEach(versionCode => {
      viewURI(
        "https://github.com/literal-io/literal/releases/tag/v" ++ versionCode,
      )
    });

  let handleClickPrivacyPolicy = () => {
    viewURI("https://literal.io/policies/privacy");
  };
  let handleClickRateThisApp = () => {
    viewURI("market://details?id=io.literal");
  };

  let handleSignOut = () => {
    let _ =
      Webview.postMessageForResult(
        Webview.WebEvent.make(~type_="AUTH_SIGN_OUT", ()),
      )
      |> Js.Promise.then_(result => {
           let _ =
             switch (
               result->Belt.Option.map(
                 Webview.WebEvent.AuthSignOutResult.decode,
               )
             ) {
             | Some(Ok({error: None})) =>
               Next.Router.replace(Routes.Authenticate.path());
               let _ =
                 setUser(
                   Providers_Authentication_User.SignedOutPromptAuthentication,
                 );
               ();
             | Some(Ok({error: Some(errorCode)})) =>
               let error =
                 Error.GenericErrorWithExtra((
                   "Unable to sign out.",
                   Js.Json.object_(
                     Js.Dict.fromList([
                       ("errorCode", errorCode->Js.Json.string),
                     ]),
                   ),
                 ));
               let _ = Error.(report(error));
               ();
             | Some(Error(error)) =>
               let _ = Error.(report(DeccoDecodeError(error)));
               ();
             | None =>
               let error =
                 Error.GenericErrorWithExtra((
                   "Unable to sign out.",
                   Js.Json.object_(
                     Js.Dict.fromList([("errorCode", Js.Json.null)]),
                   ),
                 ));
               let _ = Error.(report(error));
               ();
             };

           Js.Promise.resolve();
         });
    ();
  };

  <div
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
    ])}>
    <Header
      className={Cn.fromList([
        "py-2",
        "px-4",
        "bg-darkAccent",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
        "h-14",
        "flex",
        "flex-row",
        "items-center",
      ])}>
      <h1
        className={Cn.fromList([
          "font-sans",
          "text-lightPrimary",
          "text-lg",
          "font-bold",
        ])}>
        {React.string("Settings")}
      </h1>
    </Header>
    <main className={Cn.fromList(["py-4", "px-4"])}>
      <ProfileCard user onClickSignOut=handleSignOut />
      <AppCard
        labelClassName={Cn.fromList(["mt-12"])}
        isFlavorFoss=?{Webview.JavascriptInterface.isFlavorFoss()}
        ?nativeAppVersion
        webAppVersion=Constants.Env.webAppVersion
        onClickChangelog=handleClickChangelog
        onClickPrivacyPolicy=handleClickPrivacyPolicy
        onClickRateThisApp=handleClickRateThisApp
      />
    </main>
  </div>;
};

let page = "creators/[identityId]/settings.js";
