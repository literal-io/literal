type state =
  | Loading
  | Authenticated(AwsAmplify.Auth.CurrentUserInfo.t)
  | Unauthenticated;

