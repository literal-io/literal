let logTag = "Providers_Authentication";
type context = {
  user: Providers_Authentication_User.t,
  setUser:
    (~shouldMergeUserIdentities: bool=?, Providers_Authentication_User.t) =>
    Js.Promise.t(unit),
  refreshUser: unit => Js.Promise.t(Providers_Authentication_User.t),
};

let authenticationContext =
  React.createContext({
    user: Providers_Authentication_User.Unknown,
    setUser: (~shouldMergeUserIdentities=false, _) => Js.Promise.resolve(),
    refreshUser: () =>
      Js.Promise.resolve(Providers_Authentication_User.Unknown),
  });

module ContextProvider = {
  include React.Context;

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(authenticationContext);
};

let getUserWeb = () => Providers_Authentication_User.makeFromAmplify();
let getUserWebview = () =>
  Timer.thunkP(~label="AUTH_GET_USER", () =>
    Webview.(postMessageForResult(WebEvent.make(~type_="AUTH_GET_USER", ())))
  )
  |> Js.Promise.then_(result =>
       result
       ->Belt.Option.map(result =>
           switch (Webview.WebEvent.AuthGetUserResult.decode(result)) {
           | Ok(authGetUserResult) =>
             Providers_Authentication_User.makeFromAuthGetUserResult(
               authGetUserResult,
             )
           | Error(e) =>
             let _ = Error.(report(DeccoDecodeError(e)));
             Providers_Authentication_User.Unknown;
           }
         )
       ->Belt.Option.getWithDefault(Providers_Authentication_User.Unknown)
       ->Js.Promise.resolve
     );

[@react.component]
let make = (~children) => {
  let (user, setUser) =
    React.useState(() => Providers_Authentication_User.Unknown);
  let (mergeUserIdentitiesMutation, _s, _f) =
    ApolloHooks.useMutation(
      ~client=Providers_Apollo_Client.inst^,
      Providers_Authentication_GraphQL.MergeUserIdentitiesMutation.definition,
    );

  let handleSetUser =
      (
        ~shouldMergeUserIdentities=true,
        newUser: Providers_Authentication_User.t,
      ) => {
    switch (user, newUser) {
    | (GuestUser(guestUser), SignedInUser(signedInUser))
        when shouldMergeUserIdentities =>
      Service_Log.log(logTag, "merge user identity");
      let input =
        Providers_Authentication_GraphQL.Input.make(
          ~sourceGuestUserCredentials=guestUser.credentials,
          ~targetUserCredentials=signedInUser.cognitoUserSession,
        );
      let variables =
        Providers_Authentication_GraphQL.MergeUserIdentitiesMutation.makeVariables(
          ~input,
          (),
        );

      let mergeUserIdentitiesP = mergeUserIdentitiesMutation(~variables, ());
      let _ = setUser(_ => SignedInUserMergingIdentites(signedInUser));
      mergeUserIdentitiesP
      |> Js.Promise.then_(_ => {
           setUser(_ => newUser);
           Service_Log.log(logTag, "merge user identity complete");
           Js.Promise.resolve();
         });
    | _ => Js.Promise.resolve(setUser(_ => newUser))
    };
  };

  let refreshUser = () => {
    let userP = Webview.isWebview() ? getUserWebview() : getUserWeb();
    userP
    |> Js.Promise.then_(newUser => {
         let _ = setUser(_ => newUser);
         Js.Promise.resolve(newUser);
       })
    |> Js.Promise.catch(e => {
         let _ = setUser(_ => Unknown);
         let _ = Error.(report(PromiseError(e)));
         Js.Promise.resolve(Providers_Authentication_User.Unknown);
       });
  };

  let _ =
    React.useEffect0(() => {
      let _ = refreshUser();
      let handleAuthEvent = (ev: AwsAmplify.Hub.event(AwsAmplify.Hub.auth)) => {
        switch (ev.payload.event) {
        | "signIn" =>
          let _ = refreshUser();
          ();
        | _ => ()
        };
      };
      Service_Log.log(logTag, "refresh user");

      let _ = AwsAmplify.Hub.(listen(inst, `auth(handleAuthEvent)));
      Some(
        () => {
          let _ = AwsAmplify.Hub.(remove(inst, `auth(handleAuthEvent)));
          ();
        },
      );
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (user) {
          | Providers_Authentication_User.SignedInUser({identityId})
          | SignedInUserMergingIdentites({identityId})
          | GuestUser({identityId}) =>
            let userData =
              Js.Json.object_(
                Js.Dict.fromList([
                  ("identityId", identityId->Js.Json.string),
                ]),
              );
            let _ = SentryBrowser.setContext("user", userData);
            let _ = Service_Analytics.setUserId(Some(identityId));
            ();
          | Unknown
          | SignedOutPromptAuthentication =>
            let _ = SentryBrowser.setContext("user", Js.Json.null);
            let _ = Service_Analytics.setUserId(None);
            ();
          };
        Service_Log.log2(logTag, "user changed", user);
        None;
      },
      [|user|],
    );

  <ContextProvider value={user, refreshUser, setUser: handleSetUser}>
    children
  </ContextProvider>;
};
