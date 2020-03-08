module Vision = {
  type t;

  [@bs.new] [@bs.module "@google-cloud/vision"]
  external client: unit => t = "ImageAnnotatorClient";

  type annotateImageRequest_image = {content: Node.Buffer.t};
  type annotateImageRequest = {image: annotateImageRequest_image};

  type annotateImageResponse = Js.Json.t;

  [@bs.send]
  external documentTextDetection:
    (t, annotateImageRequest) => Js.Promise.t(annotateImageResponse) =
    "documentTextDetection";
};
