[@decco]
type t = {
  created: option(Js.Json.t),
  modified: option(Js.Json.t),
  id: option(string),
  target:
    array(
      [@decco.codec Target.codec] Target.t,
    ),
  body:
    option(
      array(
        [@decco.codec Body.codec] Body.t,
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

let makeAnnotationFromGraphQL = (~makeTarget, ~makeBody, annotation) =>
  make(
    ~id=annotation##id,
    ~target=annotation##target->Belt.Array.keepMap(makeTarget),
    ~body=?
      annotation##body->Belt.Option.map(a => a->Belt.Array.keepMap(makeBody)),
    (),
  );

let encode = t_encode;
let decode = t_decode;
