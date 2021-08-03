exception InvalidState(string);
exception ApolloEmptyData;
exception ApolloMutationError(array(ApolloHooksTypes.graphqlError));
exception PromiseError(Js.Promise.error);
exception AuthenticationRequired;
exception DeccoDecodeError(Decco.decodeError);
exception GenericErrorWithExtra((string, Js.Json.t));

let isEnabled = () =>
  Constants.Env.nodeEnv == "production"
  && !
       Webview.JavascriptInterface.isFlavorFoss()
       ->Belt.Option.getWithDefault(false);

let initialize = () =>
  if (isEnabled()) {
    SentryBrowser.(
      init(config(~dsn=Constants.Env.sentryDsn, ~normalizeDepth=10))
    );
  };

let report = exn => {
  let (error, errorContext) =
    switch (exn) {
    | InvalidState(message) => (
        "InvalidState"->Externals_Error.make->Js.Option.some,
        Some(
          SentryBrowser.makeExceptionContext(
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
    | ApolloMutationError(errors) => (
        "ApolloMutationError"->Externals_Error.make->Js.Option.some,
        errors
        ->Js.Json.stringifyAny
        ->Belt.Option.map(error =>
            SentryBrowser.makeExceptionContext(
              ~extra=
                Js.Json.object_(
                  Js.Dict.fromList([("error", error->Js.Json.string)]),
                ),
              (),
            )
          ),
      )
    | PromiseError(error) => (
        "PromiseException"->Externals_Error.make->Js.Option.some,
        error
        ->Js.Json.stringifyAny
        ->Belt.Option.map(error =>
            SentryBrowser.makeExceptionContext(
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
    | GenericErrorWithExtra((message, extra)) => (
        message->Externals_Error.make->Js.Option.some,
        Some(SentryBrowser.makeExceptionContext(~extra, ())),
      )
    | DeccoDecodeError({path, message, value}) => (
        "DeccoDecodeError"->Externals_Error.make->Js.Option.some,
        Some(
          SentryBrowser.makeExceptionContext(
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
    | _ when !isEnabled() => Js.log3("[Service_Error]", error, errorContext)
    | (Some(error), Some(errorContext)) =>
      SentryBrowser.captureExceptionWithContext(error, errorContext)
    | (Some(error), None) => SentryBrowser.captureException(error)
    | (_, _) => ()
    };
  let _ =
    switch (error) {
    | _ when !isEnabled() => ()
    | Some(_) =>
      Service_Analytics.(
        {sentryEventId: SentryBrowser.lastEventId(), type_: "ErrorService"}
        ->error
        ->track
      )
    | None => ()
    };
  ();
};
