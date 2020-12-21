[@bs.deriving abstract]
type config = {
  dsn: string,
  normalizeDepth: int,
};

[@bs.module "@sentry/browser"] external init: config => unit = "init";

type exceptionContext = {
  tags: option(Js.Dict.t(string)),
  extra: option(Js.Json.t),
  user: option(Js.Json.t),
  level: option(Js.Json.t),
  fingerprint: option(Js.Json.t),
};

let makeExceptionContext =
    (~tags=?, ~extra=?, ~user=?, ~level=?, ~fingerprint=?, ()) => {
  tags,
  extra,
  user,
  level,
  fingerprint,
};

[@bs.module "@sentry/browser"]
external captureExceptionWithContext: (Js.Exn.t, exceptionContext) => unit =
  "captureException";

[@bs.module "@sentry/browser"]
external captureException: Js.Exn.t => unit = "captureException";

[@bs.module "@sentry/browser"]
external captureMessage: string => unit = "captureMessage";

[@bs.module "@sentry/browser"]
external setContext: (string, Js.Json.t) => unit = "setContext";

[@bs.module "@sentry/browser"]
external lastEventId: unit => string = "lastEventId";
