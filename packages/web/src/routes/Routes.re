module UriEncodedCodec = {
  let encoder = s => s->Js.Global.encodeURIComponent->Js.Json.string;
  let decoder = json =>
    switch (json->Js.Json.classify) {
    | JSONString(s) => Ok(s->Js.Global.decodeURIComponent)
    | _ =>
      Error({
        Decco.path: "",
        message: "Expected JSONString for UriEncodedCodec",
        value: json,
      })
    };

  let codec: Decco.codec(string) = (encoder, decoder);
};

module CreatorsIdAnnotationsNew = {
  let staticPath = "/creators/[identityId]/annotations/new";

  let path = (~identityId) =>
    "/creators/" ++ identityId ++ "/annotations/new";

  [@decco]
  type params = {identityId: string};

  [@decco]
  type searchParams = {
    id: option(string),
    fileUrl: option(string),
  };
};

module CreatorsIdAnnotationCollectionsId = {
  let staticPath = "/creators/[identityId]/annotation-collections/[annotationCollectionIdComponent]";

  let path = (~identityId, ~annotationCollectionIdComponent) =>
    "/creators/"
    ++ Js.Global.encodeURIComponent(identityId)
    ++ "/annotation-collections/"
    ++ annotationCollectionIdComponent;

  [@decco]
  type params = {
    identityId: [@decco.codec UriEncodedCodec.codec] string,
    annotationCollectionIdComponent: string,
  };

  [@decco]
  type searchParams = {annotationId: option(string)};

  let makeSearch = search =>
    Webapi.Url.URLSearchParams.(
      [|search.annotationId->Belt.Option.map(a => ("annotationId", a))|]
      ->Belt.Array.keepMap(p => p)
      ->makeWithArray
      ->toString
    );

  let parseSearch = search => {
    annotationId: Webapi.Url.URLSearchParams.get("annotationId", search),
  };
};

module CreatorsIdSettings = {
  let staticPath = "/creators/[identityId]/settings";
  let path = (~identityId) =>
    "/creators/" ++ Js.Global.encodeURIComponent(identityId) ++ "/settings";

  [@decco]
  type params = {identityId: string};
};

module WritingId = {
  let path = (~id) => "/writing/" ++ id;
  let staticPath = "/writing/[id]";
};

module Register = {
  let path = () => "/register";
};

module Authenticate = {
  let path = () => "/authenticate";
};

module AuthenticateSignUp = {
  let path = () => "/authenticate/sign-up";
};

module AuthenticateSignIn = {
  let path = () => "/authenticate/sign-in";
};

module PolicyId = {
  let path = (~id) => "/policies/" ++ id;
  let staticPath = "/policies/[id]";
};

module Index = {
  let path = () => "/";
  let staticPath = "/";
};
