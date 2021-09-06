type attributes = {
  email: string,
  [@bs.as "email_verified"]
  emailVerified: bool,
  identities: option(string),
  sub: string,
};

type signedInUser = {
  cognitoUserSession: AmazonCognitoIdentity.UserSession.t,
  s3IdentityId: string,
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
      identityId: Some(identityId)
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

    AwsAmplify.Credentials.(setSession(inst, cognitoUserSession))
    |> Js.Promise.then_(_ => {
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

         Js.Promise.resolve(
           SignedInUser({
             identityId: username,
             s3IdentityId: identityId,
             username,
             attributes: {
               email: attributes.email,
               emailVerified: attributes.emailVerified == "true",
               identities: attributes.identities,
               sub: attributes.sub,
             },
             cognitoUserSession,
           }),
         );
       })
    |> Js.Promise.catch(err => {
         let _ = Error.(report(PromiseError(err)));
         Js.Promise.resolve(SignedOutPromptAuthentication);
       });
  | {
      credentials: Some({awsCredentials: Some(awsCredentials)}),
      identityId: Some(identityId),
    } =>
    AwsAmplify.Credentials.(
      loadCredentials(
        inst,
        Js.Promise.resolve(
          awsCredentials->Providers_Authentication_Credentials.toAmplifyCredentials,
        ),
        "guest",
        false,
        Js.Nullable.null,
      )
    )
    |> Js.Promise.then_(_ => AwsAmplify.Credentials.(setGuest(inst)))
    |> Js.Promise.then_(_ =>
         Js.Promise.resolve(
           GuestUser({credentials: awsCredentials, identityId}),
         )
       )
    |> Js.Promise.catch(err => {
         Js.log(err);
         let _ = Error.(report(PromiseError(err)));
         Js.Promise.resolve(SignedOutPromptAuthentication);
       })

  | {state: "SIGNED_OUT_FEDERATED_TOKENS_INVALID"}
  | {state: "SIGNED_OUT_USER_POOLS_TOKENS_INVALID"}
  | {state: "SIGNED_OUT"}
  | _ => Js.Promise.resolve(SignedOutPromptAuthentication)
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
             // FIXME: not sure if this is correct, should be the cognito identity id, not the username
             s3IdentityId: userInfo->AwsAmplify.Auth.CurrentUserInfo.id,
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
