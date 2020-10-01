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

[@react.component]
let make = (~children) => {
  let authentication = Hooks_CurrentUserInfo.use();

  <RawErrorBoundary
    renderErrorBoundary={() => {
      let handleRetry = () =>
        {let newHref =
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
         ()};

      <div />;
    }}
    onComponentDidCatch={error => {
      let _ = Sentry.captureException(error);
      ();
    }}>
    children
  </RawErrorBoundary>;
};
