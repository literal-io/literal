type attributes = {
  email: string,
  [@bs.as "email_verified"]
  emailVerified: bool,
  identities: option(string),
  sub: string,
};

type signedInUser = {
  cognitoUserSession: AmazonCognitoIdentity.UserSession.t,
  identityId: string,
  username: string,
  attributes,
};

type guestUser = {
  credentials: Providers_Authentication_Credentials.awsCredentials,
  identityId: string,
};

type t =
  | Unknown
  | SignedInUser(signedInUser)
  | SignedInUserMergingIdentites(signedInUser)
  | SignedOutPromptAuthentication
  | GuestUser(guestUser);

let makeFromAuthGetUserResult =
    (authGetUserResult: Webview.WebEvent.AuthGetUserResult.t) => {
  switch (authGetUserResult) {
  | {
      credentials:
        Some(Webview.WebEvent.AuthGetUserResult.{tokens: Some(tokens)}),
      username: Some(username),
      attributes: Some(attributes),
    } =>
    let cognitoUserSession =
      AmazonCognitoIdentity.(
        UserSession.make(
          UserSession.{
            idToken: IdToken.(make({idToken: tokens.idToken})),
            accessToken:
              AccessToken.(make({accessToken: tokens.accessToken})),
            refreshToken:
              RefreshToken.(make({refreshToken: tokens.refreshToken})),
          },
        )
      );
    let _ = AwsAmplify.Credentials.(setSession(inst, cognitoUserSession));
    let _ =
      cognitoUserSession
      ->AmazonCognitoIdentity.UserSession.getIdToken
      ->AmazonCognitoIdentity.IdToken.decodePayload
      ->Js.Json.decodeObject
      ->Belt.Option.flatMap(o => o->Js.Dict.get("cognito:username"))
      ->Belt.Option.flatMap(o => o->Js.Json.decodeString)
      ->Belt.Option.forEach(username => {
          AwsAmplify.Auth.(createCognitoUser(inst, username))
          ->AmazonCognitoIdentity.CognitoUser.setSignInUserSession(
              cognitoUserSession,
            )
        });

    SignedInUser({
      identityId: username,
      username,
      attributes: {
        email: attributes.email,
        emailVerified: attributes.emailVerified == "true",
        identities: attributes.identities,
        sub: attributes.sub,
      },
      cognitoUserSession,
    });
  | {
      credentials: Some({awsCredentials: Some(awsCredentials)}),
      identityId: Some(identityId),
    } =>
    GuestUser({credentials: awsCredentials, identityId})
  | {state: "SIGNED_OUT_FEDERATED_TOKENS_INVALID"}
  | {state: "SIGNED_OUT_USER_POOLS_TOKENS_INVALID"}
  | {state: "SIGNED_OUT"}
  | _ => SignedOutPromptAuthentication
  };
};

let makeFromAmplify = () =>
  Js.Promise.all2((
    AwsAmplify.Auth.(inst->currentSession),
    AwsAmplify.Auth.(inst->currentUserInfo),
  ))
  |> Js.Promise.then_(((session, userInfo)) =>
       Js.Promise.resolve((Some(session), Js.Nullable.toOption(userInfo)))
     )
  |> Js.Promise.catch(_ => Js.Promise.resolve((None, None)))
  |> Js.Promise.then_(result =>
       switch (result) {
       | (Some(session), Some(userInfo)) =>
         Js.Promise.resolve(
           SignedInUser({
             cognitoUserSession: session,
             username: userInfo->AwsAmplify.Auth.CurrentUserInfo.username,
             attributes: {
               email:
                 AwsAmplify.Auth.CurrentUserInfo.(userInfo->attributes->email),
               emailVerified:
                 AwsAmplify.Auth.CurrentUserInfo.(
                   userInfo->attributes->emailVerified
                 ),
               identities:
                 AwsAmplify.Auth.CurrentUserInfo.(
                   userInfo->attributes->identities->Js.Option.some
                 ),
               sub:
                 AwsAmplify.Auth.CurrentUserInfo.(userInfo->attributes->sub),
             },
             identityId: userInfo->AwsAmplify.Auth.CurrentUserInfo.id,
           }),
         )
       | _ =>
         AwsAmplify.Auth.(inst->currentCredentials)
         |> Js.Promise.then_(amplifyCredentials =>
              Js.Promise.resolve(
                GuestUser({
                  credentials:
                    Providers_Authentication_Credentials.fromAmplifyCredentials(
                      amplifyCredentials,
                    ),
                  identityId:
                    amplifyCredentials.AwsAmplify.Credentials.identityId,
                }),
              )
            )
         |> Js.Promise.catch(_ => Js.Promise.resolve(Unknown))
       }
     );
