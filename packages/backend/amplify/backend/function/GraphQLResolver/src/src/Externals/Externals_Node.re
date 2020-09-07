module Buffer = {
  [@bs.send]
  external toStringWithEncoding: (Node.Buffer.t, string) => string =
    "toString";
};

[@bs.val] external dirname: string = "__dirname";

module Https = {
  type result;
  type request;

  type resultData;
  [@bs.send] external toString: resultData => string = "toString";

  [@bs.send]
  external on: (result, [@bs.string] [ | `data(resultData => unit)]) => result =
    "on";

  [@bs.module "https"]
  external request: (request, (. result) => unit) => request = "request";

  [@bs.send] external write: (request, string) => unit = "write";
  [@bs.send] external end_: request => unit = "end";
};

module Url = {
  type hostname;
  [@bs.send] external hostnameToString: hostname => string = "toString";

  type url = {hostname};

  [@bs.new] [@bs.module "url"] external url: string => url = "URL";
};

module Crypto = {
  type hash;

  [@bs.module "crypto"] external makeHash: string => hash = "createHash";

  [@bs.send] external update: (hash, string) => unit = "update";
  [@bs.send] external digest: (hash, string) => string = "digest";
}
