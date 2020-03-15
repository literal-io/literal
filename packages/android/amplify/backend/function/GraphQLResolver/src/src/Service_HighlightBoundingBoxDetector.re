[@decco]
type imageBytes = {b64: string};
[@decco]
type instance = {
  key: string,
  [@decco.key "image_bytes"]
  imageBytes,
};
[@decco]
type request = {instances: array(instance)};

[@decco]
type rawPrediction = {
  [@decco.key "detection_multiclass_scores"]
  detectionMulticlassScores: array(array(float)),
  [@decco.key "detection_classes"]
  detectionClasses: array(float),
  [@decco.key "num_detection"]
  numDetections: float,
  [@decco.key "image_info"]
  imageInfo: array(int),
  [@decco.key "detection_boxes"]
  detectionBoxes: array(array(float)),
  [@decco.key "detection_scores"]
  detectionScores: array(float),
  [@decco.key "detection_classes_as_text"]
  detectionClassesAsText: array(string),
  key: string,
};

[@decco]
type response = {predictions: array(rawPrediction)};

type predictionLabel =
  | Highlight
  | HighlightEdge;
type boundingBox = {
  top: float,
  left: float,
  right: float,
  bottom: float,
};
type prediction = {
  label: predictionLabel,
  boundingBox,
  score: float,
};

/** the model downsizes images **/
let sizeX = 512;
let sizeY = 512;

let request = r =>
  Fetch.(
    fetchWithInit(
      Lib_Constants.Env.highlightBoundingBoxDetectorAPI,
      RequestInit.make(
        ~method_=Post,
        ~body=BodyInit.make(r->request_encode->Js.Json.stringify),
        (),
      ),
    )
  )
  |> Js.Promise.then_(Fetch.Response.json)
  |> Js.Promise.then_(data => {
       switch (data->response_decode) {
       | Belt.Result.Ok(response) =>
         response.predictions
         ->Belt.Array.map(p => {
             p.detectionScores
             ->Belt.Array.reduceWithIndex(
                 [||],
                 (results, score, idx) => {
                   if (score > Lib_Constants.highlightBoundingBoxScoreThreshold) {
                     let prediction = {
                       label:
                         p.detectionClassesAsText[idx] == "highlight"
                           ? Highlight : HighlightEdge,
                       score,
                       boundingBox: {
                         top: p.detectionBoxes[idx][0],
                         left: p.detectionBoxes[idx][1],
                         bottom: p.detectionBoxes[idx][2],
                         right: p.detectionBoxes[idx][3],
                       },
                     };
                     let _ = Js.Array2.push(results, prediction);
                     ();
                   };
                   results;
                 },
               )
           })
         ->Belt.Array.concatMany
         ->Js.Option.some
         ->Js.Promise.resolve
       | Belt.Result.Error(e) =>
         Js.log(e);
         Js.Promise.resolve(None);
       }
     });
