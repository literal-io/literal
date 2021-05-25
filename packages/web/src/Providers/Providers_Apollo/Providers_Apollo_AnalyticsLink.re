[@decco]
type operationError = {
  message: string,
  path: array(Js.Json.t),
  locations: Js.Json.t,
};

[@decco]
type operationResponse = {
  data: option(Js.Json.t),
  errors: option(array(operationError)),
};

let link =
  ApolloLinkAnalytics.make((operation, operationResponse) => {
    let _ =
      Service_Analytics.(
        {
          operationName: operation->ApolloLink.operationName,
          variables: operation->ApolloLink.variables,
        }
        ->graphQLOperation
        ->track
      );

    let (error, errorContext) =
      switch (operationResponse_decode(operationResponse)) {
      | Belt.Result.Ok(operationResponse) =>
        operationResponse.errors
        ->Belt.Option.map(errors => {
            let errorName =
              "AnalyticsLink - GraphQL: "
              ++ operation->ApolloLink.operationName;

            (
              Some(Externals_Error.make(errorName)),
              Some(
                SentryBrowser.{
                  extra:
                    Js.Json.object_(
                      Js.Dict.fromList([
                        (
                          "appsyncErrors",
                          errors
                          ->Belt.Array.map(error =>
                              Js.Json.object_(
                                Js.Dict.fromList([
                                  ("message", Js.Json.string(error.message)),
                                  ("path", error.path->Js.Json.array),
                                  ("locations", error.locations),
                                ]),
                              )
                            )
                          ->Js.Json.array,
                        ),
                        (
                          "operationName",
                          operation->ApolloLink.operationName->Js.Json.string,
                        ),
                        (
                          "operationVariables",
                          operation->ApolloLink.variables,
                        ),
                      ]),
                    )
                    ->Js.Option.some,
                  tags: None,
                  user: None,
                  level: None,
                  fingerprint: None,
                },
              ),
            );
          })
        ->Belt.Option.getWithDefault((None, None))
      | Belt.Result.Error(error) =>
        let errorMessage = "AnalyticsLink - Decode: " ++ error.message;
        (
          Some(Externals_Error.make(errorMessage)),
          Some(
            SentryBrowser.{
              extra:
                Js.Json.object_(
                  Js.Dict.fromList([
                    (
                      "operationName",
                      operation->ApolloLink.operationName->Js.Json.string,
                    ),
                    ("operationVariables", operation->ApolloLink.variables),
                    ("operationResponse", operationResponse),
                  ]),
                )
                ->Js.Option.some,
              tags: None,
              user: None,
              level: None,
              fingerprint: None,
            },
          ),
        );
      };

    let _ =
      switch (error, errorContext) {
      | _ when Constants.Env.nodeEnv != "production" => ()
      | (Some(error), Some(errorContext)) =>
        SentryBrowser.captureExceptionWithContext(error, errorContext)
      | (Some(error), None) => SentryBrowser.captureException(error)
      | (_, _) => ()
      };
    let _ =
      switch (error) {
      | _ when Constants.Env.nodeEnv != "production" => ()
      | Some(_) =>
        Service_Analytics.(
          {
            sentryEventId: SentryBrowser.lastEventId(),
            type_: "AnalyticsLink - GraphQL",
          }
          ->error
          ->track
        )
      | None => ()
      };
    ();
  });
