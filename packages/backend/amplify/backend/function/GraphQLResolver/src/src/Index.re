[%raw "require('isomorphic-fetch')"];

let _ = AwsAmplify.(inst->configure(Lib.Constants.awsAmplifyConfig));

let handler = event => {
  Js.log(Js.Json.stringifyAny(event));
  let res =
    switch (event->Lib.Lambda.event_decode) {
    | Belt.Result.Error(e) =>
      Js.log("Unable to decode event.");
      Js.Exn.raiseError(e.message);
    | Belt.Result.Ok(
        {typeName: "Mutation", fieldName: "createAnnotationFromExternalTarget"} as ctx
      ) =>
      QueryResolver.CreateAnnotationFromExternalTarget.resolver(ctx)
    | _ =>
      Js.log("Resolver not found.");
      Js.Exn.raiseError("Resolver not found.");
    };

  res
  |> Js.Promise.then_(res => {
       Js.log2("result: ", Js.Json.stringifyAny(res));
       Js.Promise.resolve(res);
     });
};
