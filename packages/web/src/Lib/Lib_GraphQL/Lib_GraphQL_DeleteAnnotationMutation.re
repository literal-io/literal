module Input = {
  let make = (~creatorUsername, ~id) => {
    "creatorUsername": creatorUsername,
    "id": id,
  };
};

module Apollo = {
  let updateCache = (~annotation, ~currentUser) => {
    let tagAnnotationCollectionIds =
      annotation##body
      ->Belt.Option.getWithDefault([||])
      ->Belt.Array.keepMap(body =>
          switch (body) {
          | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
            Some(body##id)
          | _ => None
          }
        )
      ->Belt.Array.map(Js.Promise.resolve);

    let sourceAnnotationCollectionIds =
      annotation##target
      ->Belt.Array.keepMap(target => {
          let externalTarget =
            switch (target) {
            | `ExternalTarget(target) => Some(target)
            | `SpecificTarget(target) =>
              switch (target##source) {
              | `ExternalTarget(target) => Some(target)
              | _ => None
              }
            | _ => None
            };

          externalTarget->Belt.Option.flatMap(externalTarget =>
            switch (externalTarget##format) {
            | Some(`TEXT_HTML) => Some(externalTarget##externalTargetId)
            | _ => None
            }
          );
        })
      ->Belt.Array.map(id =>
          Lib_GraphQL.AnnotationCollection.makeId(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~label=id,
          )
        );

    let _ =
      Belt.Array.concat(
        tagAnnotationCollectionIds,
        sourceAnnotationCollectionIds,
      )
      |> Js.Promise.all
      |> Js.Promise.then_(annotationCollectionIds => {
           let _ =
             annotationCollectionIds->Belt.Array.forEach(
               annotationCollectionId => {
               Lib_GraphQL_AnnotationCollection.Apollo.removeAnnotationFromCollection(
                 ~annotationId=annotation##id,
                 ~currentUser,
                 ~annotationCollectionId,
               )
             });
           Js.Promise.resolve();
         });
    ();
  };
};
