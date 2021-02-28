module Input = {
  let make = (~creatorUsername, ~id) => {
    "creatorUsername": creatorUsername,
    "id": id,
  };
};

module Apollo = {
  let updateCache = (~annotation, ~currentUser) => {
    annotation##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
          Some(body##id)
        | _ => None
        }
      )
    ->Belt.Array.forEach(annotationCollectionId => {
        Lib_GraphQL_AnnotationCollection.Apollo.removeAnnotationFromCollection(
          ~annotationId=annotation##id,
          ~currentUser,
          ~annotationCollectionId,
        )
      });
  };
};
