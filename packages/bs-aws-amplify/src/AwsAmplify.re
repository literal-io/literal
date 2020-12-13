/**
 * Amplify Typescript types: https://github.com/aws-amplify/amplify-js/blob/d5efee16181e108da52598f13b2eb05c15320244/packages/amazon-cognito-identity-js/index.d.ts
 */
module Config = {
  [@bs.deriving abstract]
  type cloudLogicCustom = {
    name: string,
    mutable endpoint: string,
    region: string,
  };

  [@bs.deriving abstract]
  type t = {
    [@bs.as "aws_project_region"]
    projectRegion: string,
    [@bs.as "aws_user_pools_id"]
    userPoolsId: string,
    [@bs.as "aws_user_pools_web_client_id"]
    userPoolsWebClientId: string,
    [@bs.as "aws_appsync_region"]
    appSyncRegion: string,
    [@bs.as "aws_appsync_authenticationType"]
    appSyncAuthenticationType: string,
    [@bs.as "aws_appsync_graphqlEndpoint"]
    appSyncGraphqlEndpoint: string,
    [@bs.as "aws_user_files_s3_bucket"]
    userFilesS3Bucket: string,
    [@bs.as "aws_user_files_s3_bucket_region"]
    userFilesS3BucketRegion: string,
    [@bs.as "aws_cloud_logic_custom"] [@bs.optional]
    cloudLogicCustom: array(cloudLogicCustom),
  };
  let make = t;
};

module Credentials = {
  type t;

  [@bs.module "@aws-amplify/core"] external inst: t = "Credentials";

  [@bs.send]
  external setSession:
    (t, AmazonCognitoIdentity.userSession, [@bs.as "session"] _) => unit =
    "set";
};

module Auth = {
  module JwtToken = {
    type t;

    external unsafeOfString: string => t = "%identity";
  };

  module CognitoIdToken = {
    type t;
    [@bs.send] external getJwtToken: t => JwtToken.t = "getJwtToken";
  };

  module CognitoUserSession = {
    type t;
    [@bs.send] external getIdToken: t => CognitoIdToken.t = "getIdToken";
  };

  module CurrentUserInfo = {
    [@bs.deriving accessors]
    type attributes = {
      email: string,
      [@bs.as "email_verified"]
      emailVerified: bool,
      identities: string,
      sub: string,
    };

    [@bs.deriving accessors]
    type t = {
      id: option(string),
      username: string,
      attributes,
    };
  };

  type t;

  [@bs.module "@aws-amplify/auth"] external inst: t = "default";

  [@bs.send] external configure: (t, Config.t) => unit = "configure";

  [@bs.send]
  external currentSession: t => Js.Promise.t(CognitoUserSession.t) =
    "currentSession";

  [@bs.send]
  external currentCredentials: t => Js.Promise.t(Credentials.t) =
    "currentCredentials";

  [@bs.send]
  external currentUserInfo:
    t => Js.Promise.t(Js.Nullable.t(CurrentUserInfo.t)) =
    "currentUserInfo";

  type federatedSignInOptions = {
    provider: string,
    customState: option(string),
  };
  [@bs.send]
  external federatedSignInWithOptions: (t, federatedSignInOptions) => unit =
    "federatedSignIn";

  type federatedSignInResponse = {
    [@bs.as "expires_at"]
    expiresAt: option(int),
    [@bs.as "identity_id"]
    identityId: option(string),
    token: string,
  };

  [@bs.send]
  external federatedSignInWithResponse:
    (t, string, federatedSignInResponse) => unit =
    "federatedSignIn";

  [@bs.send]
  external createCognitoUser: (t, string) => AmazonCognitoIdentity.cognitoUser =
    "createCognitoUser";
};

module Storage = {
  type t;

  [@bs.module "@aws-amplify/storage"] external inst: t = "default";
  [@bs.send] external configure: (t, Config.t) => unit = "configure";

  [@bs.send] external get: (t, string) => Js.Promise.t(string) = "get";

  type putResult = {key: string};
  type putConfig = {
    contentType: string,
    level: string,
  };

  [@bs.send]
  external putBlob:
    (t, string, Webapi.Blob.t, option(putConfig)) => Js.Promise.t(putResult) =
    "put";

  [@bs.send]
  external putFile:
    (t, string, Webapi.File.t, option(putConfig)) => Js.Promise.t(putResult) =
    "put";

  type getConfig = {
    level: string, // private|protected|public
    download: bool,
  };

  type getResult = {
    [@bs.as "Body"]
    body: Node.Buffer.t,
  };

  [@bs.send]
  external getWithConfig: (t, string, getConfig) => Js.Promise.t(getResult) =
    "get";
};

module Api = {
  type t;

  [@bs.module "@aws-amplify/api"] external inst: t = "default";
  [@bs.send] external configure: (t, Config.t) => unit = "configure";

  /** graphql **/

  [@bs.deriving abstract]
  type graphqlOperation = {
    query: string,
    variables: Js.Json.t,
  };

  [@bs.send]
  external graphql: (t, graphqlOperation) => Js.Promise.t(Js.Json.t) =
    "graphql";

  /** rest **/

  [@bs.deriving abstract]
  type response('a, 'b) = {
    data: 'a,
    status: int,
    statusText: string,
    headers: Js.t('b),
  };

  [@bs.send]
  external post:
    (
      t,
      string,
      string,
      option({
        .
        "body": option(Js.Json.t),
        "headers": option('b),
        "withCredentials": bool,
      })
    ) =>
    Js.Promise.t(response('a, 'b)) =
    "post";

  let post = (~apiName, ~path, ~options=?, inst) =>
    post(inst, apiName, path, options);
};

module Hub = {
  type t;

  [@bs.module "@aws-amplify/core"] external inst: t = "Hub";

  type payload('a) = {
    event: string,
    data: option('a),
    message: option(string),
  };
  type event('a) = {payload: payload('a)};

  type auth = {event: string};

  [@bs.send]
  external listen: (t, [@bs.string] [ | `auth(event(auth) => unit)]) => unit =
    "listen";

  [@bs.send]
  external listenWithChannel: (t, string, event(Js.Json.t) => unit) => unit =
    "listen";

  [@bs.send]
  external remove: (t, [@bs.string] [ | `auth(event(auth) => unit)]) => unit =
    "remove";

  [@bs.send]
  external removeWithChannel: (t, string, event(Js.Json.t) => unit) => unit =
    "remove";

  [@bs.send]
  external dispatch: (t, string, payload(Js.Json.t)) => unit = "dispatch";
};

module Cache = {
  type t;

  [@bs.module "@aws-amplify/cache"] external inst: t = "default";
};

type t;
[@bs.module "@aws-amplify/core"] external inst: t = "default";
[@bs.send] external configure: (t, Config.t) => unit = "configure";
