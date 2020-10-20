type accessTokenInput = {
  [@bs.as "AccessToken"]
  accessToken: string,
};
type accessToken;

[@bs.new] [@bs.module "amazon-cognito-identity-js"]
external makeAccessToken: accessTokenInput => accessToken = "CognitoIdToken";

type refreshTokenInput = {
  [@bs.as "RefreshToken"]
  refreshToken: string,
};
type refreshToken;
[@bs.new] [@bs.module "amazon-cognito-identity-js"]
external makeRefreshToken: refreshTokenInput => refreshToken =
  "CognitoRefreshToken";

type idToken;
[@bs.send]
external decodePayload: idToken => Js.Dict.t(string) = "decodePayload";
type idTokenInput = {
  [@bs.as "IdToken"]
  idToken: string,
};
[@bs.new] [@bs.module "amazon-cognito-identity-js"]
external makeIdToken: idTokenInput => idToken = "CognitoIdToken";

type userSessionInput = {
  [@bs.as "IdToken"]
  idToken,
  [@bs.as "AccessToken"]
  accessToken,
  [@bs.as "RefreshToken"]
  refreshToken,
};
type userSession;

[@bs.new] [@bs.module "amazon-cognito-identity-js"]
external makeUserSession: userSessionInput => userSession =
  "CognitoUserSession";
[@bs.send] external getIdToken: userSession => idToken = "getIdToken";

type cognitoUser;
[@bs.send]
external setSignInUserSession: (cognitoUser, userSession) => unit =
  "setSignInUserSession";
