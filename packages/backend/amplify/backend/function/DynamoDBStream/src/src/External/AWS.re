type request = {
  [@bs.as "method"]
  mutable method_: string,
  mutable body: string,
  mutable headers: Js.Dict.t(string),
  mutable host: string
};

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

