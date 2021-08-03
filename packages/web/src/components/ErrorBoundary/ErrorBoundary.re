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
  <RawErrorBoundary
    renderErrorBoundary={() => <ErrorDisplay />}
    onComponentDidCatch={error => {
      let _ = SentryBrowser.captureException(error);
      ();
    }}>
    children
  </RawErrorBoundary>;
};
