[@decco]
type t = {
  created: option(Js.Json.t),
  modified: option(Js.Json.t),
  id: option(string),
  target:
    array(
      [@decco.codec Lib_WebView_Model_Target.codec] Lib_WebView_Model_Target.t,
    ),
  body:
    option(
      array(
        [@decco.codec Lib_WebView_Model_Body.codec] Lib_WebView_Model_Body.t,
      ),
    ),
  [@decco.default "Annotation"] [@decco.key "__typename"]
  typename: string,
};

let make = (~id=?, ~target, ~body=?, ~created=?, ~modified=?, ()) => {
  id,
  target,
  body,
  typename: "Annotation",
  created:
    created
    ->Belt.Option.getWithDefault(
        Js.Date.(make()->toISOString)->Js.Json.string,
      )
    ->Js.Option.some,
  modified:
    modified
    ->Belt.Option.getWithDefault(
        Js.Date.(make()->toISOString)->Js.Json.string,
      )
    ->Js.Option.some,
};

let encode = t_encode;
let decode = t_decode;
