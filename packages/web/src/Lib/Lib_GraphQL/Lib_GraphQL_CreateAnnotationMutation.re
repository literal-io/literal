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

  let toCacheAnnotation = input =>
    Js.Dict.fromList([
      ("id", input##id->Js.Json.string),
      ("creatorUsername", input##creatorUsername->Js.Json.string),
      (
        "context",
        input##context->Belt.Array.map(c => c->Js.Json.string)->Js.Json.array,
      ),
      (
        "motivation",
        input##motivation
        ->Belt.Option.map(m =>
            m
            ->Belt.Array.map(m =>
                m->Lib_GraphQL_Motivation.toString->Js.Json.string
              )
            ->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      ("__typename", "Annotation"->Js.Json.string),
      (
        "modified",
        input##modified->Belt.Option.getWithDefault(Js.Json.null),
      ),
      ("created", input##created->Belt.Option.getWithDefault(Js.Json.null)),
      (
        "body",
        input##body
        ->Belt.Option.map(b =>
            b
            ->Belt.Array.keepMap(Lib_GraphQL_AnnotationBodyInput.toCache)
            ->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "target",
        input##target
        ->Belt.Array.keepMap(Lib_GraphQL_AnnotationTargetInput.toCache)
        ->Js.Json.array,
      ),
    ])
    ->Js.Json.object_;
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
        let cacheAnnotation = input->Input.toCacheAnnotation;

        let onCreateAnnotationCollection =
          createAnnotationCollection
            ? () =>
                Lib_GraphQL_Agent.(readCache(makeId(~currentUser)))
                ->Belt.Option.map(Js.Promise.resolve)
                ->Belt.Option.getWithDefault(
                    Lib_GraphQL_Agent.makeCache(~currentUser),
                  )
                |> Js.Promise.then_(agent =>
                     QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.makeCache(
                       ~label=textualBody##value,
                       ~annotations=[|cacheAnnotation|],
                       ~agent,
                     )
                     ->Js.Option.some
                     ->Js.Promise.resolve
                   )
            : (() => Js.Promise.resolve(None));

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
          let cacheAnnotation = input->Input.toCacheAnnotation;

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
        let shouldCreateAnnotationCollection =
          createAnnotationCollection
          || annotationCollectionId
          == Lib_GraphQL.AnnotationCollection.(
               makeIdFromComponent(
                 ~creatorUsername=
                   currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                 ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
                 (),
               )
             );

        let onCreateAnnotationCollection =
          shouldCreateAnnotationCollection
            ? () =>
                Lib_GraphQL_Agent.(readCache(makeId(~currentUser)))
                ->Belt.Option.map(Js.Promise.resolve)
                ->Belt.Option.getWithDefault(
                    Lib_GraphQL_Agent.makeCache(~currentUser),
                  )
                |> Js.Promise.then_(agent =>
                     Js.Dict.get(textualBodyById, annotationCollectionId)
                     ->Belt.Option.map(textualBody =>
                         QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.makeCache(
                           ~label=textualBody##value,
                           ~annotations,
                           ~agent,
                         )
                       )
                     ->Js.Promise.resolve
                   )
            : (() => Js.Promise.resolve(None));

        Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationsToCollection(
          ~annotations,
          ~annotationCollectionId,
          ~currentUser,
          ~onCreateAnnotationCollection,
        );
      });
  };
};
