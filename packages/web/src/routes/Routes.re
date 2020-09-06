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
  let path = (~creatorUsername) =>
    CreatorsIdAnnotations.path(~creatorUsername) ++ "/new";

  [@decco]
  type queryParams = {
    id: option(string),
    initialPhaseState: option(Containers_NoteEditor_New_Types.phase),
  };
};

module Authenticate = {
  let path = () => "/authenticate";
};
