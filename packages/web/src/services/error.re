exception InvalidState(string);

exception ApolloEmptyData;

exception PromiseError(Js.Promise.error);

exception AuthenticationRequired;

let report = exn => {
  Js.Console.trace();
  Js.Console.log(exn);

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
    };

  let _ =
    switch (error, errorContext) {
    //| _ when Constants.Env.nodeEnv != "production" => ()
    | (Some(error), Some(errorContext)) =>
      Sentry.captureExceptionWithContext(error, errorContext)
    | (Some(error), None) => Sentry.captureException(error)
    | (_, _) => ()
    };
  let _ =
    switch (error) {
    //| _ when Constants.Env.nodeEnv != "production" => ()
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
