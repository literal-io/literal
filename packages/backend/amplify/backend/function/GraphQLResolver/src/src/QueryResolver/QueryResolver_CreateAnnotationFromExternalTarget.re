open Service;

let cropImageToHighlight = (predictions, imageBuffer) => {
  let gm = Externals_Gm.client(. imageBuffer, Externals_UUID.makeV4());

  Externals_Gm.size(gm)
  |> Js.Promise.then_(size => {
       let resizedX =
         float_of_int(Service_HighlightBoundingBoxDetector.sizeX);
       let resizedY =
         float_of_int(Service_HighlightBoundingBoxDetector.sizeY);

       /** ratio of original image to resized square used for prediction **/
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
             p2.score -. p1.score < 0. ?
               Js.Math.floor(p2.score -. p1.score) :
               Js.Math.ceil(p2.score -. p1.score)
           )
         ->Belt.Array.partition(
             fun
             | {label: Highlight} => true
             | _ => false,
           );

       let _ =
         highlightEdges
         ->Belt.Array.forEach(({boundingBox}) => {
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
         ->Belt.Option.map(({boundingBox}) =>
             gm
             ->Externals.Gm.crop(
                 int_of_float(boundingBox.right -. boundingBox.left),
                 int_of_float(boundingBox.bottom -. boundingBox.top),
                 int_of_float(boundingBox.left),
                 int_of_float(boundingBox.top),
               )
           );

       gm->Externals_Gm.toBuffer("PNG");
     })
  |> Js.Promise.then_(r => r->Js.Option.some->Js.Promise.resolve)
  |> Js.Promise.catch(e => {
       Js.log2("[Error] cropImageToHighlight: ", e);
       Js.Promise.resolve(None);
     });
};

[@decco]
[@bs.deriving accessors]
type externalTargetInput = {
  id: string,
  format: option(string),
  language: option(string),
  processingLanguage: option(string),
  textDirection: option(string),
  [@decco.key "type"]
  type_: option(string),
  accessibility: option(array(string)),
  rights: option(array(string)),
};

[@decco]
[@bs.deriving accessors]
type input = {
  creatorUsername: string,
  annotationId: option(string),
  externalTarget: externalTargetInput,
};

[@decco]
[@bs.deriving accessors]
type arguments = {input};

let parseTextFromExternalTarget = input => {
  let eligibleRegex =
    Js.Re.fromString(
      "^s3:\/\/"
      ++ Lib_Constants.Env.storageLiteralStorageBucketName
      ++ "\/private\/.+\/screenshots\/.+$",
    );
  let eligible = eligibleRegex->Js.Re.test_(input.externalTarget.id);

  if (eligible) {
    let {bucket, key}: Externals_AmazonS3URI.t =
      Externals_AmazonS3URI.make(input.externalTarget.id);
    Externals_AWS.S3.(
      getObject(makeClient({version: "2006-03-01"}), {bucket, key})->promise
    )
    ->Lib_OptionPromise.fromPromise
    ->Lib_OptionPromise.map(s3GetResult => {
        let imageBuffer = s3GetResult.Externals_AWS.S3.body;
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
        |> Js.Promise.then_(r => r->Js.Option.some->Js.Promise.resolve);
      })
    ->Lib_OptionPromise.mapOption(textDetectionResponse => {
        textDetectionResponse
        ->Belt.Array.get(0)
        ->Belt.Option.map(r =>
            Externals_GoogleCloud.Vision.(
              r
              ->fullTextAnnotation
              ->text
              ->Js.String2.replaceByRe([%re "/\\n/g"], " ")
              ->Js.Promise.resolve
            )
          );
      });
  } else {
    Js.log("ID not eligible, skipping.");
    Js.Promise.resolve(None);
  };
};

let renameTargetIds = [%raw {|
  function (annotation) {
    return {
      ...annotation,
      target: (annotation.target || []).map(target => ({
        ...target,
        id: target.externalTargetId || target.textualTargetId
      }))
    }
  }
|}];

module CreateAnnotationMutation = [%graphql
  {|
    mutation CreateAnnotation($input: CreateAnnotationInput!) {
      createAnnotation(input: $input) {
        annotation {
          id
          context
          type
          target {
            ... on ExternalTarget {
              __typename
              externalTargetId: id
              format
              language
              processingLanguage
              textDirection
              type
              accessibility
              rights
            }
            ... on TextualTarget {
              __typename
              textualTargetId: id
              format
              language
              processingLanguage
              textDirection
              accessibility
              rights
              value
            }
          }
          created
          generated
          modified
          audience {
            id
            type
          }
          motivation
          via
          canonical
          stylesheet {
            type
            value
          }
          creatorUsername
        }
      }
    }
  |}
];

let createAnnotation = (~input, ~text) => {
  let id =
    switch (input.annotationId) {
    | Some(annotationId) => annotationId
    | None =>
      let hash = Externals_Node.Crypto.makeHash("sha256");
      let _ = Externals_Node.Crypto.update(hash, text);
      let digest = Externals_Node.Crypto.digest(hash, "hex");

      Lib_Constants.apiOrigin
      ++ "/creators/"
      ++ input.creatorUsername
      ++ "/annotations/"
      ++ digest;
    };

  let mutation =
    CreateAnnotationMutation.make(
      ~input={
        "context": [|"http://www.w3.org/ns/anno.jsonld"|],
        "type": [|`ANNOTATION|],
        "id": id,
        "body":
          Some([|
            {
              "textualBody":
                Some({
                  "id":
                    Some(
                      Lib_AnnotationCollection.(
                        makeIdFromComponent(
                          ~creatorUsername=input.creatorUsername,
                          ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
                          (),
                        )
                      ),
                    ),
                  "value": Lib_AnnotationCollection.recentAnnotationCollectionLabel,
                  "purpose": Some([|`TAGGING|]),
                  "rights": None,
                  "accessibility": None,
                  "format": Some(`TEXT_PLAIN),
                  "textDirection": Some(`LTR),
                  "language": Some(`EN_US),
                  "processingLanguage": Some(`EN_US),
                  "type": Some(`TEXTUAL_BODY),
                }),
              "externalBody": None,
              "choiceBody": None,
              "specificBody": None,
            },
          |]),
        "target": [|
          {
            "textualTarget": None,
            "externalTarget":
              Some({
                "format": Some(`TEXT_PLAIN),
                "language": Some(`EN_US),
                "processingLanguage": Some(`EN_US),
                "textDirection": Some(`LTR),
                "accessibility": input.externalTarget.accessibility,
                "rights": input.externalTarget.rights,
                "id": input.externalTarget.id,
                "type": Some(`IMAGE),
              }),
          },
          {
            "textualTarget":
              Some({
                "format": Some(`TEXT_PLAIN),
                "language": Some(`EN_US),
                "processingLanguage": Some(`EN_US),
                "textDirection": Some(`LTR),
                "accessibility": input.externalTarget.accessibility,
                "rights": input.externalTarget.rights,
                "value": text,
                "id": None,
              }),
            "externalTarget": None,
          },
        |],
        "created": None,
        "modified": None,
        "generated": None,
        "audience": None,
        "canonical": None,
        "stylesheet": None,
        "via": None,
        "motivation": Some([|`HIGHLIGHTING|]),
        "creatorUsername": input.creatorUsername,
        "annotationGeneratorId": None,
      },
      (),
    );
  GraphQL.request(
    ~query=mutation##query,
    ~variables=mutation##variables,
    ~operationName="CreateAnnotation",
  );
};

let resolver = (ctx: Lib_Lambda.event) =>
  switch (ctx.arguments->arguments_decode->Belt.Result.map(input)) {
  | Belt.Result.Ok(input) =>
    input
    ->parseTextFromExternalTarget
    ->Lib_OptionPromise.map(text =>
        createAnnotation(~input, ~text)
        |> Js.Promise.then_(r => {
             let parseResult =
               switch (r->Js.Json.decodeObject) {
               | Some(o) =>
                 switch (o->Js.Dict.get("data"), o->Js.Dict.get("errors")) {
                 | (_, Some(errors)) =>
                   Js.log2(
                     "[Error] CreateAnnotationMutation errors:",
                     Js.Json.stringifyAny(errors),
                   );
                   None;
                 | (Some(data), _) =>
                  data
                  ->Js.Json.decodeObject
                  ->Belt.Option.flatMap(o => o->Js.Dict.get("createAnnotation"))
                  ->Belt.Option.flatMap(o => o->Js.Json.decodeObject)
                  ->Belt.Option.flatMap(o => o->Js.Dict.get("annotation"))
                  ->Belt.Option.map(o => renameTargetIds(o))
                 | (None, None) =>
                   Js.log2(
                     "[Error] Unhandled CreateAnnotationMutation response:",
                     Js.Json.stringifyAny(r),
                   );
                   None;
                 }
               | None =>
                 Js.log2(
                   "[Error] Unable to parse CreateAnnotationMutation result:",
                   Js.Json.stringifyAny(r),
                 );
                 None;
               };
             Js.Promise.resolve(parseResult);
           })
      )
  | Belt.Result.Error(e) =>
    Js.log2("[Error] Unable to decode arguments", e);
    Js.Promise.resolve(None);
  };
