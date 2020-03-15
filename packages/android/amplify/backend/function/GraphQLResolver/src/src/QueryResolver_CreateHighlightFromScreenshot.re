// TODO: install imagemagick lambda layer: https://github.com/serverlesspub/imagemagick-aws-lambda-2

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

let cropImageToHighlight = (predictions, imageBuffer) => {
  let gm = Externals_Gm.client(imageBuffer, Externals_UUID.makeV4());
  Externals_Gm.size(gm)
  |> Js.Promise.then_(size => {
       let scaleX =
         float_of_int(
           size.Externals_Gm.width / Service_HighlightBoundingBoxDetector.sizeX,
         );
       let scaleY =
         float_of_int(
           size.height / Service_HighlightBoundingBoxDetector.sizeY,
         );
       let {boundingBox}: Service_HighlightBoundingBoxDetector.prediction =
         predictions->Belt.Array.reduce(predictions[0], (memo, p) =>
           if (p.Service_HighlightBoundingBoxDetector.score > memo.score) {
             {
               ...p,
               boundingBox: {
                 top: p.boundingBox.top *. scaleY,
                 bottom: p.boundingBox.bottom *. scaleY,
                 left: p.boundingBox.left *. scaleX,
                 right: p.boundingBox.right *. scaleX,
               },
             };
           } else {
             memo;
           }
         );
       // TODO: remove highlight edges
       gm
       ->Externals_Gm.crop(
           int_of_float(boundingBox.right -. boundingBox.left),
           int_of_float(boundingBox.top -. boundingBox.bottom),
           int_of_float(boundingBox.left),
           int_of_float(boundingBox.right),
         )
       ->Externals_Gm.toBuffer("PNG");
     })
  |> Js.Promise.then_(r => r->Js.Option.some->Js.Promise.resolve);
};

let parseTextFromScreenshot = screenshotId => {
  let query = GetScreenshotQuery.make(~screenshotId, ());
  let op =
    AwsAmplify.Api.graphqlOperation(
      ~query=query##query,
      ~variables=query##variables,
    );
  Lib_OptionPromise.fromPromise(AwsAmplify.Api.(graphql(inst, op)))
  ->Lib_OptionPromise.mapOption(r =>
      r
      ->GetScreenshotQuery.parse
      ->(d => d##getScreenshot)
      ->Belt.Option.map(s => {
          AwsAmplify.Storage.(
            getWithConfig(
              inst,
              s##file##key,
              {level: "public", download: true},
            )
          )
        })
    )
  ->Lib_OptionPromise.map(s3GetResult => {
      let imageBuffer = s3GetResult.AwsAmplify.Storage.data;
      let requestData =
        Service_HighlightBoundingBoxDetector.{
          instances: [|
            {
              imageBytes: {
                b64:
                  Externals_Node.Buffer.toStringWithEncoding(
                    imageBuffer,
                    "base64",
                  ),
              },
              key: Externals_UUID.makeV4(),
            },
          |],
        };
      Service_HighlightBoundingBoxDetector.request(requestData)
      ->Lib_OptionPromise.map(r => r->cropImageToHighlight(imageBuffer));
    })
  ->Lib_OptionPromise.map(croppedImageBuffer => {
      Externals_GoogleCloud.Vision.(
        client()
        ->documentTextDetection({
            image: {
              content: croppedImageBuffer,
            },
          })
      )
      |> Js.Promise.then_(r => r->Js.Option.some->Js.Promise.resolve)
    })
  ->Lib_OptionPromise.mapOption(textDetectionResponse => {
      textDetectionResponse
      ->Belt.Array.get(0)
      ->Belt.Option.map(r =>
          Externals_GoogleCloud.Vision.(
            r
            ->fullTextAnnotation
            ->Belt.Array.map(text)
            ->Js.String.concatMany(" ")
            ->Js.Promise.resolve
          )
        )
    });
};

module CreateHighlightMutation = [%graphql
  {|
    mutation CreateHighlight($input: CreateHighlightInput!) {
      createHighlight(input: $input) {
        id
        createdAt
        text
        note
      }
    }
  |}
];

let createHighlight = (text, screenshotId) => {
  let mutation =
    CreateHighlightMutation.make(
      ~input={
        "text": text,
        "note": None,
        "id": None,
        "createdAt": None,
        "highlightScreenshotId": screenshotId,
      },
      (),
    );
  AwsAmplify.Api.(
    graphqlOperation(~query=mutation##query, ~variables=mutation##variables)
    |> graphql(inst)
  );
};

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
    input.screenshotId
    ->parseTextFromScreenshot
    ->Lib_OptionPromise.map(text =>
        createHighlight(text, input.screenshotId)
        ->Lib_OptionPromise.fromPromise
      )
  | Belt.Result.Error(e) =>
    Js.log2("Unable to decode arguments", e);
    Js.Promise.resolve(None);
  };