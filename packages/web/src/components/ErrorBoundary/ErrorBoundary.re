let _ = Sentry.(init(config(~dsn=Constants.Env.sentryDsn)));

module RawErrorBoundary = {
  [@bs.module "./raw-error-boundary.js"] [@react.component]
  external make:
    (
      ~onComponentDidCatch: Js.Exn.t => unit,
      ~renderErrorBoundary: unit => React.element,
      ~children: React.element
    ) =>
    React.element =
    "default";
};

module ErrorDisplay = {
  [@react.component]
  let make = () => {
    let authentication = Hooks_CurrentUserInfo.use();
    let handleRetry = () => {
      let newHref =
        switch (authentication) {
        | Authenticated(currentUser) =>
          Routes.CreatorsIdAnnotationCollectionsId.path(
            ~creatorUsername=currentUser.username,
            ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
          )
        | _ => Routes.Authenticate.path()
        };
      let href = Webapi.Dom.(window->Window.location->Location.href);
      let _ =
        if (href == newHref) {
          Webapi.Dom.(window->Window.location->Location.reload);
        } else {
          Webapi.Dom.(window->Window.location->Location.setHref(newHref));
        };
      ();
    };

    let _ =
      React.useEffect0(() => {
        let _ =
          Service_Analytics.(
            track(Page({route: "/error", asPath: "/error", query: None}))
          );
        None;
      });

    <div
      className={Cn.fromList([
        "w-full",
        "h-full",
        "bg-black",
        "flex",
        "flex-col",
        "justify-between",
        "px-6",
        "pb-4",
        "pt-16",
      ])}>
      <div>
        <Svg
          icon=Svg.logo
          placeholderViewBox="0 0 24 24"
          className={Cn.fromList(["pointer-events-none", "w-16", "h-16"])}
        />
      </div>
      <div
        className={Cn.fromList([
          "flex",
          "flex-col",
          "justify-start",
          "items-start",
          "mb-16",
        ])}>
        <h1
          className={Cn.fromList([
            "text-lightPrimary",
            "font-sans",
            "font-bold",
            "leading-none",
            "text-xl",
            "mb-4",
            "underline",
          ])}>
          {React.string("Something went wrong.")}
        </h1>
        <p
          className={Cn.fromList([
            "text-lightPrimary",
            "font-sans",
            "leading-relaxed",
            "text-lg",
            "mb-6",
          ])}>
          {React.string(
             "A report of this event has been recorded and we are looking into resolving it.",
           )}
        </p>
        <MaterialUi.Button
          onClick={_ => {
            let _ =
              Service_Analytics.(
                track(Click({action: "retry", label: Some("Retry")}))
              );
            let _ = handleRetry();
            ();
          }}
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-white"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }
          fullWidth=false
          size=`Large
          classes={MaterialUi.Button.Classes.make(
            ~root=Cn.fromList(["py-4"]),
            ~label=
              Cn.fromList([
                "text-white",
                "font-sans",
                "font-bold",
                "text-lg",
                "leading-none",
                "italic",
                "px-6",
              ]),
            ~outlined=Cn.fromList(["border-white"]),
            (),
          )}
          variant=`Outlined>
          {React.string("Retry")}
        </MaterialUi.Button>
      </div>
    </div>;
  };
};

[@react.component]
let make = (~children) => {
  <RawErrorBoundary
    renderErrorBoundary={() => <ErrorDisplay />}
    onComponentDidCatch={error => {
      let _ = Sentry.captureException(error);
      ();
    }}>
    children
  </RawErrorBoundary>;
};
