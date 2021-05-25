type context = {
  onNext: (unit => unit) => unit,
  setOnNext: option(unit => unit) => unit,
};

let context = React.createContext({onNext: _ => (), setOnNext: _ => ()});

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
  let preparedNavigation = React.useRef(false);
  let (onNext, setOnNext) = React.useState(_ => None);
  let router = Next.Router.useRouter();

  let handleOnNext = defaultAction => {
    let cb = onNext->Belt.Option.getWithDefault(defaultAction);
    let _ = setOnNext(_ => None);
    let _ = cb();
    ();
  };
  let _ =
    React.useEffect3(
      () => {
        if (!preparedNavigation.current) {
          let _ = setOnNext(_ => None);
          ();
        };
        preparedNavigation.current = false;
        None;
      },
      (router.asPath, router.route, router.query),
    );

  let handleSetOnNext = action => {
    setOnNext(_ => action);
    preparedNavigation.current = true;
    ();
  };

  <ContextProvider value={onNext: handleOnNext, setOnNext: handleSetOnNext}>
    children
  </ContextProvider>;
};
