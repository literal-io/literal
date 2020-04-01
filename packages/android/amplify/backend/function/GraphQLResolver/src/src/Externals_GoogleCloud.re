module Vision = {
  type t;

  [@bs.new] [@bs.module "@google-cloud/vision"]
  external client: unit => t = "ImageAnnotatorClient";

  type annotateImageRequest_image = {content: Node.Buffer.t};
  type annotateImageRequest = {image: annotateImageRequest_image};

  // https://cloud.google.com/vision/docs/reference/rest/v1/AnnotateImageResponse#Page
  type page = Js.Json.t;

  [@bs.deriving accessors]
  type fullTextAnnotation = {
    pages: array(page),
    text: string,
  };

  [@bs.deriving accessors]
  type annotateImageResponse = {
    fullTextAnnotation: array(fullTextAnnotation),
  };
  type documentTextDetectionResponse = array(annotateImageResponse);

  [@bs.send]
  external documentTextDetection:
    (t, annotateImageRequest) => Js.Promise.t(documentTextDetectionResponse) =
    "documentTextDetection";
};
