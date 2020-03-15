type t;

external make: Js.Promise.t(option('a)) => t = "%identity";

let fromOption = (o: option('a)) => o->Js.Promise.resolve;
let fromPromise = p =>
  p
  |> Js.Promise.then_(r => r->Js.Option.some->fromOption)
  |> Js.Promise.catch(e => {
       Js.log(e);
       fromOption(None);
     });

let map = (op, cb): Js.Promise.t(option('a)) =>
  op
  |> Js.Promise.then_(
       fun
       | Some(v) => cb(v)
       | None => fromOption(None),
     );

let flatMap = (op, cb): Js.Promise.t(option('a)) =>
  switch (op) {
  | Some(o) => o->cb
  | None => fromOption(None)
  };

let mapOption = (op, cb): Js.Promise.t(option('a)) =>
  op
  |> Js.Promise.then_(
       fun
       | Some(v) =>
         v
         ->cb
         ->Belt.Option.map(v =>
             v
             |> Js.Promise.then_(r => {r->Js.Option.some->Js.Promise.resolve})
           )
         ->Belt.Option.getWithDefault(fromOption(None))
       | None => fromOption(None),
     );
