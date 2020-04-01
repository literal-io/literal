[%raw "require('isomorphic-fetch')"];

let _ = AwsAmplify.(inst->configure(Lib.Constants.awsAmplifyConfig));

let handler = event => {
  Js.log(Js.Json.stringifyAny(event));
  switch (event->Lib.Lambda.event_decode) {
  | Belt.Result.Ok(
      {typeName: "Mutation", fieldName: "createHighlightFromScreenshot"} as ctx,
    ) =>
    QueryResolver.CreateHighlightFromScreenshot.resolver(ctx)
  | Belt.Result.Error(e) =>
    Js.log("Unable to decode event.");
    Js.Exn.raiseError(e.message);
  | _ =>
    Js.log("Resolver not found.");
    Js.Exn.raiseError("Resolver not found.");
  };
};
