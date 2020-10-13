[@bs.deriving abstract]
type config = {dsn: string};

[@bs.module "@sentry/browser"] external init: config => unit = "init";

[@bs.module "@sentry/browser"]
external captureException: Js.Exn.t => unit = "captureException";

[@bs.module "@sentry/browser"]
external captureMessage: string => unit = "captureMessage";

[@bs.module "@sentry/browser"]
external setContext: (string, Js.Json.t) => unit = "setContext";
