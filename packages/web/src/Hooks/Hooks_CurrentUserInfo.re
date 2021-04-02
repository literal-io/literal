open Hooks_CurrentUserInfo_Types;

let currentUserInfoWeb = () =>
  AwsAmplify.Auth.(currentUserInfo(inst))
  |> Js.Promise.then_(currentUser =>
       currentUser->Js.Nullable.toOption->Js.Promise.resolve
     );

let currentUserInfoWebview = () =>
  Timer.thunkP(~label="AUTH_GET_USER_INFO", () =>
    Webview.(
      postMessageForResult(WebEvent.make(~type_="AUTH_GET_USER_INFO", ()))
    )
  )
  |> Js.Promise.then_(result => {
       result
       ->Belt.Option.flatMap(data => {
           switch (Webview.WebEvent.AuthGetUserInfoResult.decode(data)) {
           | Belt.Result.Ok(userInfo) =>
             Some(
               AwsAmplify.Auth.CurrentUserInfo.{
                 id: Some(userInfo.id),
                 username: userInfo.username,
                 attributes: {
                   email: userInfo.attributes.email,
                   emailVerified: userInfo.attributes.emailVerified === "true",
                   identities: userInfo.attributes.identities,
                   sub: userInfo.attributes.sub,
                 },
               },
             )
           | Belt.Result.Error(_) => None
           }
         })
       ->Js.Promise.resolve
     });

let use = () => {
  let Providers_Authentication.{currentUserInfo, setCurrentUserInfo} =
    React.useContext(Providers_Authentication.authenticationContext);

  let checkAuthenticationState = () => {
    let isWebview =
      LiteralWebview.inst
      ->Belt.Option.map(LiteralWebview.isWebview)
      ->Belt.Option.getWithDefault(false);
    let _ =
      (isWebview ? currentUserInfoWebview : currentUserInfoWeb)()
      |> Js.Promise.then_(currentUser => {
           let newAuthenticationState =
             switch (currentUser) {
             | Some(currentUser) => Authenticated(currentUser)
             | None => Unauthenticated
             };
           setCurrentUserInfo(newAuthenticationState);
           Js.Promise.resolve();
         })
      |> Js.Promise.catch(_ => {
           setCurrentUserInfo(Unauthenticated);
           Js.Promise.resolve();
         });
    ();
  };

  let _ =
    React.useEffect0(() => {
      let _ = checkAuthenticationState();

      let handleAuthEvent = (ev: AwsAmplify.Hub.event(AwsAmplify.Hub.auth)) => {
        switch (ev.payload.event) {
        | "signIn" =>
          let _ = checkAuthenticationState();
          ();
        | _ => ()
        };
      };

      let handleWebviewEvent = (ev: AwsAmplify.Hub.event(Js.Json.t)) => {
        switch (ev.payload.event) {
        | "AUTH_SIGN_IN_RESULT" =>
          let _ = checkAuthenticationState();
          ();
        | _ => ()
        };
      };

      let _ = AwsAmplify.Hub.(listen(inst, `auth(handleAuthEvent)));
      let _ =
        AwsAmplify.Hub.(
          listenWithChannel(inst, "webview", handleWebviewEvent)
        );
      Some(
        () => {
          let _ = AwsAmplify.Hub.(remove(inst, `auth(handleAuthEvent)));
          let _ =
            AwsAmplify.Hub.(
              removeWithChannel(inst, "webview", handleWebviewEvent)
            );
          ();
        },
      );
    });

  currentUserInfo;
};
