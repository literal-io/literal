type state =
  | Loading
  | Authenticated(AwsAmplify.Auth.CurrentUserInfo.t)
  | Unauthenticated;

let use = () => {
  let (authenticationState, setAuthenticationState) =
    React.useState(() => Loading);

  let _ =
    React.useEffect0(() => {
      let _ =
        AwsAmplify.Auth.(currentUserInfo(inst))
        |> Js.Promise.then_(currentUser => {
             let _ =
               switch (currentUser->Js.Nullable.toOption) {
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
