%raw
"require('isomorphic-fetch')";
let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

let fragmentTypes = [%raw "require('../../fragment-types.json')"];

/** FIXME: more sophisticated authTokens cache **/
let authTokens = ref(None);
let authenticatedClientAuthOptions = {
  Webview.isWebview()
    ? AwsAppSync.Client.authWithCognitoUserPools(~jwtToken=() => {
        switch (authTokens^) {
        | Some(tokens) =>
          tokens.Webview.WebEvent.idToken
          ->AwsAmplify.Auth.JwtToken.unsafeOfString
          ->Js.Promise.resolve
        | None =>
          Timer.thunkP(~label="AUTH_GET_TOKENS", () =>
            Webview.(
              postMessageForResult(WebEvent.make(~type_="AUTH_GET_TOKENS"))
            )
          )
          |> Js.Promise.then_(result => {
               switch (result) {
               | Some(data) =>
                 switch (Webview.WebEvent.authGetTokensResult_decode(data)) {
                 | Belt.Result.Ok(tokens) =>
                   authTokens := Some(tokens);
                   let cognitoSession =
                     AmazonCognitoIdentity.(
                       makeUserSession({
                         idToken: makeIdToken({idToken: tokens.idToken}),
                         accessToken:
                           makeAccessToken({accessToken: tokens.accessToken}),
                         refreshToken:
                           makeRefreshToken({
                             refreshToken: tokens.refreshToken,
                           }),
                       })
                     );
                   let credentials =
                     AwsAmplify.Credentials.(
                       setSession(inst, cognitoSession)
                     );
                   let _ =
                     cognitoSession
                     ->AmazonCognitoIdentity.getIdToken
                     ->AmazonCognitoIdentity.decodePayload
                     ->Js.Dict.get("cognito:username")
                     ->Belt.Option.forEach(username => {
                         AwsAmplify.Auth.(createCognitoUser(inst, username))
                         ->AmazonCognitoIdentity.setSignInUserSession(
                             cognitoSession,
                           )
                       });

                   tokens.idToken
                   ->AwsAmplify.Auth.JwtToken.unsafeOfString
                   ->Js.Promise.resolve;
                 | Belt.Result.Error(_) =>
                   let _ = Next.Router.replace(Routes.Authenticate.path());
                   Js.Promise.reject(Error.AuthenticationRequired);
                 }
               | None =>
                 let _ = Next.Router.replace(Routes.Authenticate.path());
                 Js.Promise.reject(Error.AuthenticationRequired);
               }
             })
        }
      })
    : AwsAppSync.Client.authWithCognitoUserPools(~jwtToken=() =>
        AwsAmplify.(
          Auth.(inst->currentSession)
          |> Js.Promise.then_(s =>
               s
               ->Auth.CognitoUserSession.getIdToken
               ->Auth.CognitoIdToken.getJwtToken
               ->Js.Promise.resolve
             )
        )
      );
};

let unauthenticatedClientAuthOptions =
  AwsAppSync.Client.authWithIAM(~credentials=() =>
    AwsAmplify.Auth.(inst->currentCredentials)
  );

/**
 * FIXME: https://github.com/awslabs/aws-mobile-appsync-sdk-js/issues/450#issuecomment-522264166
 */
let appSyncLinkOptions =
  AwsAppSync.Client.{
    url:
      AwsAmplify.(
        Constants.awsAmplifyConfig->Config.appSyncGraphqlEndpointGet
      ),
    region: AwsAmplify.(Constants.awsAmplifyConfig->Config.appSyncRegionGet),
    auth: authenticatedClientAuthOptions,
    disableOffline: true,
    complexObjectsCredentials: () =>
      AwsAmplify.Auth.(inst->currentCredentials),
    mandatorySignIn: false,
  };

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

let analyticsLink =
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
                Sentry.{
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
            Sentry.{
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
        Sentry.captureExceptionWithContext(error, errorContext)
      | (Some(error), None) => Sentry.captureException(error)
      | (_, _) => ()
      };
    let _ =
      switch (error) {
      | _ when Constants.Env.nodeEnv != "production" => ()
      | Some(_) =>
        Service_Analytics.(
          {
            sentryEventId: Sentry.lastEventId(),
            type_: "AnalyticsLink - GraphQL",
          }
          ->error
          ->track
        )
      | None => ()
      };
    ();
  });

let appSyncLink = AwsAppSync.Client.createAppSyncLink(appSyncLinkOptions);

let appSyncCache =
  ApolloInMemoryCache.createInMemoryCache(
    ~fragmentMatcher=
      ApolloInMemoryCache.introspectionFragmentMatcher({
        "introspectionQueryResultData": fragmentTypes,
      }),
    (),
  );

let client =
  AwsAppSync.Client.(
    makeWithOptions(
      appSyncLinkOptions,
      {
        link: Some(ApolloLinks.from([|analyticsLink, appSyncLink|])),
        cache: Some(appSyncCache),
      },
    )
  );

[@react.component]
let make = (~render) =>
  <ReasonApollo.Provider client>
    <AwsAppSync.Rehydrated
      render={({rehydrated}) =>
        <ApolloHooks.Provider client>
          {render(~rehydrated)}
        </ApolloHooks.Provider>
      }
    />
  </ReasonApollo.Provider>;
