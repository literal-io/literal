exception InvalidState(string);
exception ApolloEmptyData;
exception PromiseError(Js.Promise.error);
exception AuthenticationRequired;
exception DeccoDecodeError(Decco.decodeError);

let report = exn => {
  let (error, errorContext) =
    switch (exn) {
    | InvalidState(message) => (
        "InvalidState"->Externals_Error.make->Js.Option.some,
        Some(
          Sentry.makeExceptionContext(
            ~extra=
              Js.Json.object_(
                Js.Dict.fromList([("message", Js.Json.string(message))]),
              ),
            (),
          ),
        ),
      )
    | ApolloEmptyData => (
        "ApolloEmptyData"->Externals_Error.make->Js.Option.some,
        None,
      )
    | PromiseError(error) => (
        "PromiseException"->Externals_Error.make->Js.Option.some,
        error
        ->Js.Json.stringifyAny
        ->Belt.Option.map(error =>
            Sentry.makeExceptionContext(
              ~extra=
                Js.Json.object_(
                  Js.Dict.fromList([("error", error->Js.Json.string)]),
                ),
              (),
            )
          ),
      )
    | AuthenticationRequired => (
        "AuthenticationRequired"->Externals_Error.make->Js.Option.some,
        None,
      )
    | DeccoDecodeError({path, message, value}) => (
        "DeccoDecodeError"->Externals_Error.make->Js.Option.some,
        Some(
          Sentry.makeExceptionContext(
            ~extra=
              Js.Json.object_(
                Js.Dict.fromList([
                  ("path", path->Js.Json.string),
                  ("message", message->Js.Json.string),
                  ("value", value),
                ]),
              ),
            (),
          ),
        ),
      )
    };

  let _ =
    switch (error, errorContext) {
    | _ when Constants.Env.nodeEnv != "production" => Js.log(error)
    | (Some(error), Some(errorContext)) =>
      Sentry.captureExceptionWithContext(error, errorContext)
    | (Some(error), None) => Sentry.captureException(error)
    | (_, _) => ()
    };
  let _ =
    switch (error) {
    | _ when Constants.Env.nodeEnv != "production" => ()
    | Some(_) =>
      Service_Analytics.(
        {sentryEventId: Sentry.lastEventId(), type_: "ErrorService"}
        ->error
        ->track
      )
    | None => ()
    };
  ();
};
