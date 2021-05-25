module JwtToken = {
  type t;
  external toString: t => string = "%identity";
};

module RefreshToken = {
  type t;
  type input = {
    [@bs.as "RefreshToken"]
    refreshToken: string,
  };

  [@bs.new] [@bs.module "amazon-cognito-identity-js"]
  external make: input => t = "CognitoRefreshToken";
};

module AccessToken = {
  type t;
  type input = {
    [@bs.as "AccessToken"]
    accessToken: string,
  };

  [@bs.send] external getJwtToken: t => JwtToken.t = "getJwtToken";

  [@bs.new] [@bs.module "amazon-cognito-identity-js"]
  external make: input => t = "CognitoIdToken";
};

module IdToken = {
  type t;
  type input = {
    [@bs.as "IdToken"]
    idToken: string,
  };

  [@bs.send] external getJwtToken: t => JwtToken.t = "getJwtToken";
  [@bs.send] external decodePayload: t => Js.Json.t = "decodePayload";

  [@bs.new] [@bs.module "amazon-cognito-identity-js"]
  external make: input => t = "CognitoIdToken";
};

module UserSession = {
  type t;
  type input = {
    [@bs.as "IdToken"]
    idToken: IdToken.t,
    [@bs.as "AccessToken"]
    accessToken: AccessToken.t,
    [@bs.as "RefreshToken"]
    refreshToken: RefreshToken.t,
  };

  [@bs.new] [@bs.module "amazon-cognito-identity-js"]
  external make: input => t = "CognitoUserSession";

  [@bs.send] external getIdToken: t => IdToken.t = "getIdToken";
  [@bs.send] external getRefreshToken: t => RefreshToken.t = "getRefreshToken";
  [@bs.send] external getAccessToken: t => AccessToken.t = "getRefreshToken";
};

module CognitoUser = {
  type t;
  [@bs.send]
  external setSignInUserSession: (t, UserSession.t) => unit =
    "setSignInUserSession";
};
