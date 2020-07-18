[%raw "require('isomorphic-fetch')"];

let handler: Lambda.handler =
  (event, ctx, cb) => {
    Js.log(Js.Json.stringifyAny(event));
    switch (event->Lambda.event_decode) {
    | Belt.Result.Ok(event) =>
      event.records
      ->Belt.Array.map(r =>
          switch (
            r.eventName,
            Lambda.tableNameFromEventARN(r.eventSourceARN),
            r.dynamodb.oldImage,
          ) {
          | _ =>
            Js.log2("Unhandled record: ", Js.Json.stringifyAny(r));
            Js.Promise.resolve();
          }
        )
      |> Js.Promise.all
      |> Js.Promise.then_(_ => {Js.Promise.resolve(None)})
      |> Js.Promise.catch(err => {
           Js.log(err);
           let exn =
             Error.EventHandlerError(
               Js.Json.stringifyAny(err)
               ->Belt.Option.getWithDefault("Promise exception"),
             );
           Js.Promise.reject(exn);
         })
    | Belt.Result.Error(error) =>
      let exn = Error.DeccoDecodeError(error);
      Error.report(exn);
      Js.Promise.reject(exn);
    };
  };
