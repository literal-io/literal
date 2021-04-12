type context = {
  currentUserInfo: Hooks_CurrentUserInfo_Types.state,
  setCurrentUserInfo: Hooks_CurrentUserInfo_Types.state => unit,
};

let authenticationContext =
  React.createContext({
    currentUserInfo: Hooks_CurrentUserInfo_Types.Loading,
    setCurrentUserInfo: _ => (),
  });

module ContextProvider = {
  include React.Context;

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };

  let make = React.Context.provider(authenticationContext);
};

[@react.component]
let make = (~children) => {
  let (currentUserInfo, setCurrentUserInfo) =
    React.useState(() => Hooks_CurrentUserInfo_Types.Loading);

  let setCurrentUserInfoWithSideEffects = currentUserInfo => {
    let _ = setCurrentUserInfo(_ => currentUserInfo);
    let _ =
      switch (currentUserInfo) {
      | Hooks_CurrentUserInfo_Types.Authenticated(currentUser) =>
        let username = currentUser->AwsAmplify.Auth.CurrentUserInfo.username;
        let userData =
          Js.Json.object_(
            Js.Dict.fromList([("username", username->Js.Json.string)]),
          );
        let _ = Sentry.setContext("user", userData);
        let _ = Service_Analytics.setUserId(username);
        ();
      | _ => ()
      };
    ();
  };

  <ContextProvider
    value={
      currentUserInfo,
      setCurrentUserInfo: setCurrentUserInfoWithSideEffects,
    }>
    children
  </ContextProvider>;
};
