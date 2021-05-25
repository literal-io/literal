let getWebviewCredentials = () =>
  Timer.thunkP(~label="AUTH_GET_CREDENTIALS", () =>
    Webview.postMessageForResult(
      Webview.WebEvent.make(~type_="AUTH_GET_CREDENTIALS", ()),
    )
  )
  |> Js.Promise.then_(credentialsJson => {
       credentialsJson
       ->Belt.Option.flatMap(credentialsJson =>
           switch (Providers_Apollo_Credentials.decode(credentialsJson)) {
           | Ok(credentials) => Some(credentials)
           | Error(e) =>
             let _ = Error.(report(DeccoDecodeError(e)));
             None;
           }
         )
       ->Js.Promise.resolve
     });

let makeAuthenticatedClientAuthOptions = cognitoUserSession =>
  AwsAppSync.Client.authWithCognitoUserPools(~jwtToken=() =>
    cognitoUserSession
    ->AmazonCognitoIdentity.UserSession.getIdToken
    ->AmazonCognitoIdentity.IdToken.getJwtToken
    ->Js.Promise.resolve
  );

let makeGuestClientAuthOptions = credentials =>
  AwsAppSync.Client.authWithIAM(~credentials=() =>
    credentials
    ->Providers_Authentication_Credentials.toAmplifyCredentials
    ->Js.Promise.resolve
  );

let makeAppSyncLinkOptions = (~auth) =>
  AwsAppSync.Client.{
    url:
      AwsAmplify.(
        Constants.awsAmplifyConfig->Config.appSyncGraphqlEndpointGet
      ),
    region: AwsAmplify.(Constants.awsAmplifyConfig->Config.appSyncRegionGet),
    auth,
    disableOffline: true,
    complexObjectsCredentials: () =>
      AwsAmplify.Auth.(inst->currentCredentials),
    mandatorySignIn: false,
  };

let makeLink = appSyncLinkOptions =>
  AwsAppSync.Client.createAppSyncLink(appSyncLinkOptions);
