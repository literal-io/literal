module Input = {
  let make = (~creatorUsername, ~annotationId, ~externalTarget) => {
    "creatorUsername": creatorUsername,
    "annotationId": annotationId,
    "externalTarget": externalTarget,
  };
};
