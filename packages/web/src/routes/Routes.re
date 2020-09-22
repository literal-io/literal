module CreatorsIdAnnotationsNew = {
  let staticPath = "/creators/[creatorUsername]/annotations/new";

  let path = (~creatorUsername) =>
    "/creators/" ++ creatorUsername ++ "/annotations/new";

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
    "/creators/"
    ++ creatorUsername
    ++ "/annotation-collections/"
    ++ annotationCollectionIdComponent;

  [@decco]
  type params = {
    creatorUsername: string,
    annotationCollectionIdComponent: string,
  };
};

module Authenticate = {
  let path = () => "/authenticate";
};
