type request = {
  [@bs.as "method"]
  mutable method_: string,
  mutable body: string,
  mutable headers: Js.Dict.t(string),
  mutable host: string,
};

external toNodeRequest: request => Externals_Node.Https.request = "%identity";

[@bs.new] [@bs.module "aws-sdk"]
external httpRequest: (string, string) => request = "HttpRequest";

module Config = {
  type credentials;

  [@bs.val] [@bs.scope "config"] [@bs.module "aws-sdk"]
  external credentials: credentials = "credentials";
};

module Util = {
  type date;
  [@bs.val] [@bs.scope ("util", "date")] [@bs.module "aws-sdk"]
  external getDate: unit => date = "getDate";
};

module Signer = {
  type t;

  [@bs.send]
  external addAuthorization: (t, Config.credentials, Util.date) => unit =
    "addAuthorization";
  [@bs.new] [@bs.scope "Signers"] [@bs.module "aws-sdk"]
  external make: (request, string, bool) => t = "V4";
};

module S3 = {
  type client;

  type request('a);
  [@bs.send] external promise: request('a) => Js.Promise.t('a) = "promise";

  type clientConfig = { version: string };
  [@bs.new] [@bs.module "aws-sdk"] external makeClient: clientConfig => client = "S3";

  type getObjectParams = {
    [@bs.as "Bucket"] bucket: string,
    [@bs.as "Key"] key: string
  };
  type s3Object = {
    [@bs.as "Body"] body: Node.Buffer.t
  };
  [@bs.send] external getObject: (client, getObjectParams) => request(s3Object) = "getObject";
}
