type context = {
  setIsVisible: bool => unit,
  isVisible: bool,
};

let context = React.createContext({setIsVisible: _ => (), isVisible: false});

module ContextProvider = {
  include React.Context;

  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
  let make = React.Context.provider(context);
};

[@react.component]
let make = (~children) => {
  let (isVisible, setIsVisible) = React.useState(_ => false);
  let router = Next.Router.useRouter();

  let _ =
    React.useEffect3(
      () => {
        let _ = setIsVisible(_ => true);
        None;
      },
      (router.asPath, router.route, router.query),
    );

  let handleSetIsVisible = newIsVisible => setIsVisible(_ => newIsVisible);

  <ContextProvider value={isVisible, setIsVisible: handleSetIsVisible}>
    children
  </ContextProvider>;
};
