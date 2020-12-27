module CreatorsIdAnnotationsNew = {
  let staticPath = "/creators/[creatorUsername]/annotations/new";

  let path = (~creatorUsername) =>
    "/creators/" ++ creatorUsername ++ "/annotations/new";

  [@decco]
  type params = {creatorUsername: string};

  [@decco]
  type searchParams = {
    id: option(string),
    fileUrl: option(string),
  };
};

module CreatorsIdAnnotationCollectionsId = {
  let staticPath = "/creators/[creatorUsername]/annotation-collections/[annotationCollectionIdComponent]";

  let path = (~creatorUsername, ~annotationCollectionIdComponent) =>
    "/creators/"
    ++ creatorUsername
    ++ "/annotation-collections/"
    ++ annotationCollectionIdComponent;

  [@decco]
  type params = {
    creatorUsername: string,
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

module WritingId = {
  let path = (~id) => "/writing/" ++ id;
  let staticPath = "/writing/[id]";
};

module Authenticate = {
  let path = () => "/authenticate";
};

module Index = {
  let path = () => "/";
  let staticPath = "/";
};
