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
  let gm = Externals_Gm.client(. imageBuffer, Externals_UUID.makeV4());

  Externals_Gm.size(gm)
  |> Js.Promise.then_(size => {
       let resizedX =
         float_of_int(Service_HighlightBoundingBoxDetector.sizeX);
       let resizedY =
         float_of_int(Service_HighlightBoundingBoxDetector.sizeY);

       // ratio of original image to resized square used for prediction
       let relativeScaleX = float_of_int(size.Externals_Gm.width) /. resizedX;
       let relativeScaleY = float_of_int(size.height) /. resizedY;

       let (highlights, highlightEdges) =
         predictions
         ->Belt.Array.map(p =>
             Service.HighlightBoundingBoxDetector.{
               ...p,
               boundingBox: {
                 top: p.boundingBox.top *. resizedY *. relativeScaleY,
                 bottom: p.boundingBox.bottom *. resizedY *. relativeScaleY,
                 left: p.boundingBox.left *. resizedX *. relativeScaleX,
                 right: p.boundingBox.right *. resizedX *. relativeScaleX,
               },
             }
           )
         ->Js.Array2.sortInPlaceWith((p1, p2) =>
             p2.score -. p1.score < 0.
               ? Js.Math.floor(p2.score -. p1.score)
               : Js.Math.ceil(p2.score -. p1.score)
           )
         ->Belt.Array.partition(
             fun
             | {label: Highlight} => true
             | _ => false,
           );

       let _ =
         highlightEdges->Belt.Array.forEach(({boundingBox}) => {
           let _ =
             gm
             ->Externals.Gm.fill("black")
             ->Externals.Gm.drawRectangle(
                 int_of_float(boundingBox.left),
                 int_of_float(boundingBox.top),
                 int_of_float(boundingBox.right),
                 int_of_float(boundingBox.bottom),
               );
           let _ =
             gm
             ->Externals.Gm.region(
                 int_of_float(boundingBox.right -. boundingBox.left),
                 int_of_float(boundingBox.bottom -. boundingBox.top),
                 int_of_float(boundingBox.left),
                 int_of_float(boundingBox.top),
               )
             ->Externals.Gm.transparent("black")
             ->Externals.Gm.out("+region");
           ();
         });

       let _ =
         highlights
         ->Belt.Array.get(0)
         ->Belt.Option.map(({boundingBox}) => {
             gm->Externals.Gm.crop(
               int_of_float(boundingBox.right -. boundingBox.left),
               int_of_float(boundingBox.bottom -. boundingBox.top),
               int_of_float(boundingBox.left),
               int_of_float(boundingBox.top),
             )
           });

       gm->Externals_Gm.toBuffer("PNG");
     })
  |> Js.Promise.then_(r => {
       let _ = [%raw
         {|
          (function () {
            const path = "/tmp/crop-" + (new Date()).toISOString() + ".png"
            require('fs').writeFileSync(path, r)
            console.log("cropped", path)
          })()
        |}
       ];
       r->Js.Option.some->Js.Promise.resolve;
     })
  |> Js.Promise.catch(e => {
       Js.log(e);
       Js.Promise.resolve(None);
     });
};

let parseTextFromScreenshot = screenshotId => {
  let query = GetScreenshotQuery.make(~screenshotId, ());
  let op =
    AwsAmplify.Api.graphqlOperation(
      ~query=query##query,
      ~variables=query##variables,
    );
  Lib_OptionPromise.fromPromise(AwsAmplify.Api.(graphql(inst, op)))
  ->Lib_OptionPromise.mapOption(r => {
      r
      ->Js.Json.decodeObject
      ->Belt.Option.flatMap(o => o->Js.Dict.get("data"))
      ->Belt.Option.flatMap(d =>
          d->GetScreenshotQuery.parse->(r => r##getScreenshot)
        )
      ->Belt.Option.map(s => {
          let unleveledKey =
            s##file##key
            ->Js.String2.split("/")
            ->Js.Array2.sliceFrom(1)
            ->Js.Array2.joinWith("/");

          AwsAmplify.Storage.(
            getWithConfig(
              inst,
              unleveledKey,
              {level: "public", download: true},
            )
          );
        })
    })
  ->Lib_OptionPromise.map(s3GetResult => {
      let imageBuffer = s3GetResult.AwsAmplify.Storage.body;
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
        client({keyFilename: Lib_Constants.gcloudServiceAccountFilename})
        ->documentTextDetection({
            image: {
              content: croppedImageBuffer,
            },
          })
      )
      |> Js.Promise.then_(r => r->Js.Option.some->Js.Promise.resolve)
    })
  ->Lib_OptionPromise.mapOption(textDetectionResponse => {
      Js.log2(
        "textDetectionResponse",
        Js.Json.stringifyAny(textDetectionResponse),
      );
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
        );
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
  note: option(string),
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
