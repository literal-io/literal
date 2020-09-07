type context = {
  currentUserInfo: Hooks_CurrentUserInfo_Types.state,
  setCurrentUserInfo:
    (Hooks_CurrentUserInfo_Types.state => Hooks_CurrentUserInfo_Types.state) =>
    unit,
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

  <ContextProvider value={currentUserInfo, setCurrentUserInfo}>
    children
  </ContextProvider>;
};
