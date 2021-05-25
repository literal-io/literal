type config = {
  dsn: string,
  normalizeDepth: option(int),
};

[@bs.module "@sentry/node"] external init: config => unit = "init";

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

[@bs.module "@sentry/node"]
external captureExceptionWithContext: (Js.Exn.t, exceptionContext) => unit =
  "captureException";

[@bs.module "@sentry/node"]
external captureException: Js.Exn.t => unit = "captureException";

[@bs.module "@sentry/node"]
external captureMessage: string => unit = "captureMessage";

[@bs.module "@sentry/node"]
external setContext: (string, Js.Json.t) => unit = "setContext";

[@bs.module "@sentry/node"]
external lastEventId: unit => string = "lastEventId";
