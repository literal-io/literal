type dict = Js.Dict.t(string);

[@decco]
type event = {
  typeName: string,
  fieldName: string,
  arguments: Js.Json.t,
  identity: Js.Json.t,
  source: Js.Json.t,
  request: Js.Json.t,
  prev: option(Js.Json.t),
};
