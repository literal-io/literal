module CreatorsIdAnnotations = {
  let staticPath = "/creators/[creatorUsername]/annotations";

  let path = (~creatorUsername) =>
    "/creators/" ++ creatorUsername ++ "/annotations";

  [@decco]
  type params = {creatorUsername: string};
};

module CreatorsIdAnnotationsId = {
  let staticPath = "/creators/[creatorUsername]/annotations/[annotationIdComponent]";

  let path = (~creatorUsername, ~annotationIdComponent) =>
    "/creators/" ++ creatorUsername ++ "/annotations/" ++ annotationIdComponent;

  [@decco]
  type params = {
    creatorUsername: string,
    annotationIdComponent: string,
  };
};

module CreatorsIdAnnotationsNew = {
  let staticPath = "/creators/[creatorUsername]/annotations/new";

  let path = (~creatorUsername) =>
    CreatorsIdAnnotations.path(~creatorUsername) ++ "/new";

  [@decco]
  type queryParams = {
    creatorUsername: string,
    id: option(string),
    initialPhaseState: option(Containers_AnnotationEditor_New_Types.phase),
  };
};

module CreatorsIdAnnotationCollectionsId = {
  let staticPath = "/creators/[creatorUsername]/annotation-collections/[annotationCollectionIdComponent]";

  let path = (~creatorUsername, ~annotationCollectionIdComponent) =>
    "/creators/" ++ creatorUsername ++ "/annotations/" ++ annotationCollectionIdComponent;

  [@decco]
  type params = {
    creatorUsername: string,
    annotationCollectionIdComponent: string,
  };
};

module Authenticate = {
  let path = () => "/authenticate";
};
