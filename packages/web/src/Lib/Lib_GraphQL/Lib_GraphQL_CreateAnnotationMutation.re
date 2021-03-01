module Input = {
  let make =
      (
        ~id,
        ~creatorUsername,
        ~context,
        ~target,
        ~body=?,
        ~created=?,
        ~modified=?,
        ~generated=?,
        ~audience=?,
        ~canonical=?,
        ~stylesheet=?,
        ~via=?,
        ~motivation=?,
        ~annotationGeneratorId=?,
        (),
      ) => {
    "id": id,
    "creatorUsername": creatorUsername,
    "context": context,
    "type": [|`ANNOTATION|],
    "created": created,
    "modified": modified,
    "generated": generated,
    "audience": audience,
    "canonical": canonical,
    "stylesheet": stylesheet,
    "via": via,
    "motivation": motivation,
    "annotationGeneratorId": annotationGeneratorId,
    "target": target,
    "body": body,
  };

  let toAnnotation = input => {
    "__typename": "Annotation",
    "id": input##id,
    "created":
      input##created
      ->Belt.Option.getWithDefault(
          Js.Date.make()->Js.Date.toISOString->Js.Json.string,
        )
      ->Js.Option.some,
    "modified":
      input##modified
      ->Belt.Option.getWithDefault(
          Js.Date.make()->Js.Date.toISOString->Js.Json.string,
        )
      ->Js.Option.some,
    "creatorUsername": input##creatorUsername,
    "context": input##context,
    "generated": input##generated,
    "audience": input##audience,
    "canonical": input##canonical,
    "stylesheet": input##stylesheet,
    "via": input##via,
    "motivation": input##motivation,
    "annotationGeneratorId": input##annotationGeneratorId,
    "target":
      input##target
      ->Belt.Array.keepMap(Lib_GraphQL_AnnotationTargetInput.toTarget),
    "body":
      input##body
      ->Belt.Option.map(body =>
          body->Belt.Array.keepMap(Lib_GraphQL_AnnotationBodyInput.toBody)
        ),
  };
};

module Apollo = {
  let updateCache =
      (~currentUser, ~input, ~createAnnotationCollection=false, ()) =>
    input##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        body
        ->Lib_GraphQL_AnnotationBodyInput.toBody
        ->Belt.Option.flatMap(body =>
            switch (body) {
            | `TextualBody(textualBody) =>
              Lib_GraphQL.Annotation.isBodyTag(textualBody)
                ? Some(textualBody) : None
            | _ => None
            }
          )
      )
    ->Belt.Array.forEach(textualBody => {
        let cacheAnnotation =
          input
          ->Input.toAnnotation
          ->QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.parsedAnnotationToCache;
        let onCreateAnnotationCollection =
          createAnnotationCollection
            ? () =>
                QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.makeCache(
                  ~label=textualBody##value,
                  ~annotations=[|cacheAnnotation|],
                )
                ->Js.Option.some
            : (() => None);

        Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationToCollection(
          ~annotation=cacheAnnotation,
          ~currentUser,
          ~annotationCollectionId=textualBody##id,
          ~onCreateAnnotationCollection,
        );
      });

  let updateCacheMany =
      (~currentUser, ~inputs, ~createAnnotationCollection=false, ()) => {
    let textualBodyAnnotationTuples =
      inputs
      ->Belt.Array.map(input => {
          let cacheAnnotation =
            input
            ->Input.toAnnotation
            ->QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.parsedAnnotationToCache;

          input##body
          ->Belt.Option.getWithDefault([||])
          ->Belt.Array.keepMap(body =>
              body
              ->Lib_GraphQL_AnnotationBodyInput.toBody
              ->Belt.Option.flatMap(body =>
                  switch (body) {
                  | `TextualBody(body) =>
                    Lib_GraphQL.Annotation.isBodyTag(body)
                      ? Some((body, cacheAnnotation)) : None
                  | _ => None
                  }
                )
            );
        })
      ->Belt.Array.concatMany;

    let textualBodyById =
      textualBodyAnnotationTuples->Belt.Array.reduce(
        Js.Dict.empty(),
        (agg, (textualBody, _)) => {
          if (Js.Dict.get(agg, textualBody##id)->Js.Option.isNone) {
            let _ = Js.Dict.set(agg, textualBody##id, textualBody);
            ();
          };
          agg;
        },
      );

    textualBodyAnnotationTuples
    ->Belt.Array.reduce(
        Js.Dict.empty(),
        (agg, (textualBody, cacheAnnotation)) => {
          let annotations =
            agg
            ->Js.Dict.get(textualBody##id)
            ->Belt.Option.map(a => Belt.Array.concat(a, [|cacheAnnotation|]))
            ->Belt.Option.getWithDefault([|cacheAnnotation|]);
          let _ = Js.Dict.set(agg, textualBody##id, annotations);
          agg;
        },
      )
    ->Js.Dict.entries
    ->Belt.Array.forEach(((annotationCollectionId, annotations)) => {
        let onCreateAnnotationCollection =
          createAnnotationCollection
            ? () =>
                Js.Dict.get(textualBodyById, annotationCollectionId)
                ->Belt.Option.map(textualBody =>
                    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.makeCache(
                      ~label=textualBody##value,
                      ~annotations,
                    )
                  )
            : (() => None);

        Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationsToCollection(
          ~annotations,
          ~annotationCollectionId,
          ~currentUser,
          ~onCreateAnnotationCollection,
        );
      });
  };
};
