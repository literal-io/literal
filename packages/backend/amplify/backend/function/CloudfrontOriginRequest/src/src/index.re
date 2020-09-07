[@bs.module "path"] external extname: string => string = "extname";

[@bs.deriving accessors]
type origin = {mutable path: string};

[@bs.deriving accessors]
type s3Origin = {s3: origin};

[@bs.deriving accessors]
type request = {
  origin: s3Origin,
  mutable uri: string,
};

[@bs.deriving accessors]
type cloudfrontRecord = {
  config: Js.Json.t,
  request,
};

[@bs.deriving accessors]
type record = {cf: cloudfrontRecord};

type event = {
  [@bs.as "Records"]
  records: array(record),
};

let routes = [|
  (
    [%re "/^\/creators\/.+\/annotations$/"],
    "/creators/[creatorUsername]/annotations.html"
  ),
  (
    [%re "/^\/creators\/.+\/annotations\/new$/"],
    "/creators/[creatorUsername]/annotations/new.html"
  ),
  (
    [%re "/^\/creators\/.+\/annotations\/.+$/"],
    "/creators/[creatorUsername]/annotations/[annotationIdComponent].html"
  ),
|];

let handler = (event, _ctx, cb) => {
  let req = event.records->Belt.Array.getUnsafe(0)->cf->request;

  let _ =
    switch (
      routes
      ->Belt.Array.getBy(((regex, _)) => Js.Re.test_(regex, req.uri))
    ) {
      | Some((_, path)) =>
        req.uri = path;
      | None when extname(req.uri) === "" =>
        let path =
          Js.String2.endsWith(req.uri, "/")
            ? Js.String2.substring(
                req.uri,
                ~from=0,
                ~to_=Js.String.length(req.uri) - 1,
              )
            : req.uri;
        req.uri = path ++ ".html";
      | None => ()
    };

  cb(. None, req);
};
