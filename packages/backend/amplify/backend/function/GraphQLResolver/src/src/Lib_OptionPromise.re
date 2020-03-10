type t;

external make: Js.Promise.t(option('a)) => t = "%identity";

let fromOption = (o: option('a)) => o->Js.Promise.resolve;
let fromPromise = p =>
  p
  |> Js.Promise.then_(r => r->Js.Option.some->fromOption)
  |> Js.Promise.catch(_e => fromOption(None));

let map = (op, cb) =>
  switch (op) {
  | Some(o) => o->cb->fromOption
  | None => fromOption(None)
  };
