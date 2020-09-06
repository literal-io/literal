type state =
  | Loading
  | Authenticated(AwsAmplify.Auth.CurrentUserInfo.t)
  | Unauthenticated;

let currentUserInfoWeb = () =>
  AwsAmplify.Auth.(currentUserInfo(inst))
  |> Js.Promise.then_(currentUser =>
       currentUser->Js.Nullable.toOption->Js.Promise.resolve
     );

let currentUserInfoWebview = () =>
  Timer.thunkP(~label="AUTH_GET_USER_INFO", () =>
    Webview.(
      postMessageForResult(WebEvent.make(~type_="AUTH_GET_USER_INFO"))
    )
  )
  |> Js.Promise.then_(result => {
       result
       ->Belt.Option.flatMap(data => {
           switch (Webview.WebEvent.authGetUserInfoResult_decode(data)) {
           | Belt.Result.Ok(userInfo) =>
            Js.log2("getUserInfo", userInfo);
             Some(
               AwsAmplify.Auth.CurrentUserInfo.{
                 id: Some(userInfo.id),
                 username: userInfo.username,
                 attributes: userInfo.attributes,
               },
             )
           | Belt.Result.Error(_) => None
           }
         })
       ->Js.Promise.resolve
     });

let use = () => {
  let (authenticationState, setAuthenticationState) =
    React.useState(() => Loading);

  let _ =
    React.useEffect0(() => {
      let isWebview =
        LiteralWebview.inst
        ->Belt.Option.map(LiteralWebview.isWebview)
        ->Belt.Option.getWithDefault(false);
      let _ =
        (isWebview ? currentUserInfoWebview : currentUserInfoWeb)()
        |> Js.Promise.then_(currentUser => {
             let _ =
               switch (currentUser) {
               | Some(currentUser) =>
                 setAuthenticationState(_ => Authenticated(currentUser))
               | None => setAuthenticationState(_ => Unauthenticated)
               };
             Js.Promise.resolve();
           })
        |> Js.Promise.catch(_ => {
             setAuthenticationState(_ => Unauthenticated);
             Js.Promise.resolve();
           });
      None;
    });
  authenticationState;
};
