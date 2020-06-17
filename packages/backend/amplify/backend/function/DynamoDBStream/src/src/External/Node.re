type result;
type request;

type resultData;
[@bs.send] external toString: resultData => string = "toString";

[@bs.send]
external on: (result, [@bs.string] [ | `data(resultData => unit)]) => result =
  "on";

[@bs.module "https"]
external request: (Js.t('a), (. result) => unit) => request = "request";

[@bs.send] external write: (request, string) => unit = "write";
[@bs.send] external end_: request => unit = "end";
