type result;
type request;

type resultData;
[@bs.send] external toString: resultData => string = "toString";

[@bs.send]
external on: (result, [@bs.string] [ | `data(resultData => unit)]) => result =
  "on";

[@bs.module "https"]
external request: (AWS.request, (. result) => unit) => request = "request";

[@bs.send] external write: (request, string) => unit = "write";
[@bs.send] external end_: request => unit = "end";

type hostname;
[@bs.send] external hostnameToString: hostname => string = "toString";

type url = {hostname};

[@bs.new] [@bs.module "url"] external url: string => url = "URL";
