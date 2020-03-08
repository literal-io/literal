module GetScreenshotQuery = [%graphql
  {|
    query GetScreenshot($screenshotId: ID!) {
      getScreenshot(id: $screenshotId) {
        file {
          bucket
          key
          region
        }
      }
    }
  |}
];

module CreateHighlightFromScreenshot = {
  [@decco]
  [@bs.deriving accessors]
  type argumentsInput = {
    id: option(string),
    createdAt: option(string),
    screenshotId: string,
    note: string,
  };

  [@decco]
  [@bs.deriving accessors]
  type arguments = {input: argumentsInput};

  let resolver = (ctx: Lib_Lambda.event) =>
    switch (ctx.arguments->arguments_decode->Belt.Result.map(input)) {
    | Belt.Result.Ok(input) =>
      let query =
        GetScreenshotQuery.make(~screenshotId=input.screenshotId, ());
      let op =
        AwsAmplify.Api.graphqlOperation(
          ~query=query##query,
          ~variables=query##variables,
        );
      AwsAmplify.Api.(graphql(inst, op))
      |> Js.Promise.then_(r => {
           let data = GetScreenshotQuery.parse(r);
           switch (data##getScreenshot) {
           | Some(s) =>
             AwsAmplify.Storage.(
               getWithConfig(
                 inst,
                 s##file##key,
                 {level: "public", download: true},
               )
             )
             |> Js.Promise.then_(s =>
                  Js.Promise.resolve(Some(s.AwsAmplify.Storage.data))
                )
           | None => Js.Promise.resolve(None)
           };
         })
      |> Js.Promise.then_(data => {
           data
           ->Belt.Option.map(data => {
               let enc =
                 Externals_Node.Buffer.toStringWithEncoding(data, "base64");

               Js.Promise.resolve(Some());
             })
           ->Belt.Option.getWithDefault(Js.Promise.resolve(None))
         })
      |> Js.Promise.catch(err => {Js.Promise.resolve(None)});
    | Belt.Result.Error(e) =>
      Js.log("Unable to decode arguments");
      Js.Exn.raiseError(e.message);
    };
};
