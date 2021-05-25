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
      (~identityId, ~input, ~createAnnotationCollection=false, ()) => {
    let _ =
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
              ? () => {
                  let agent =
                    Lib_GraphQL_Agent.(readCache(makeId(~identityId)))
                    ->Belt.Option.getWithDefault(
                        Lib_GraphQL_Agent.makeCache(~identityId),
                      );

                  QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.makeCache(
                    ~label=textualBody##value,
                    ~annotationCollectionType="TAG_COLLECTION",
                    ~annotationCollectionId=textualBody##id,
                    ~annotations=[|cacheAnnotation|],
                    ~agent,
                  )
                  ->Js.Option.some
                  ->Js.Promise.resolve;
                }
              : (() => Js.Promise.resolve(None));

          Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationToCollection(
            ~annotation=cacheAnnotation,
            ~identityId,
            ~annotationCollectionId=textualBody##id,
            ~annotationCollectionLabel=textualBody##value,
            ~annotationCollectionType="TAG_COLLECTION",
            ~onCreateAnnotationCollection,
          );
        });
    let _ =
      input##target
      ->Belt.Array.keepMap(target => {
          let externalTarget =
            switch (
              target##externalTarget,
              target##specificTarget,
              target##textualTarget,
            ) {
            | (Some(externalTarget), _, _) => Some(externalTarget)
            | (_, Some(specificTarget), _) =>
              switch (specificTarget##source##externalTarget) {
              | Some(externalTarget) => Some(externalTarget)
              | None => None
              }
            | _ => None
            };
          externalTarget->Belt.Option.flatMap(externalTarget =>
            switch (externalTarget##format) {
            | Some(`TEXT_HTML) => Some(externalTarget)
            | _ => None
            }
          );
        })
      ->Belt.Array.forEach(externalTarget => {
          let cacheAnnotation = input->Input.toCacheAnnotation;
          let onCreateAnnotationCollection = () => Js.Promise.resolve(None);

          Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationToCollection(
            ~annotation=cacheAnnotation,
            ~annotationCollectionType="SOURCE_COLLECTION",
            ~annotationCollectionLabel=externalTarget##id,
            ~annotationCollectionId=
              Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
                ~identityId,
                ~annotationCollectionIdComponent=externalTarget##hashId,
                (),
              ),
            ~identityId,
            ~onCreateAnnotationCollection,
          );
        });
    ();
  };

  let updateCacheMany =
      (~identityId, ~inputs, ~createAnnotationCollection=false, ()) => {
    let cacheAnnotations = inputs->Belt.Array.map(Input.toCacheAnnotation);
    let _ = {
      let textualBodyAnnotationTuples =
        inputs
        ->Belt.Array.mapWithIndex((idx, input) => {
            let cacheAnnotation = cacheAnnotations->Belt.Array.getExn(idx);
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
              ->Belt.Option.map(a =>
                  Belt.Array.concat(a, [|cacheAnnotation|])
                )
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
                   ~identityId,
                   ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
                   (),
                 )
               );

          let textualBody =
            Js.Dict.get(textualBodyById, annotationCollectionId)
            ->Belt.Option.getExn;

          let onCreateAnnotationCollection =
            shouldCreateAnnotationCollection
              ? () => {
                  let agent =
                    Lib_GraphQL_Agent.(readCache(makeId(~identityId)))
                    ->Belt.Option.getWithDefault(
                        Lib_GraphQL_Agent.makeCache(~identityId),
                      );
                  QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.makeCache(
                    ~annotationCollectionId,
                    ~annotationCollectionType="TAG_COLLECTION",
                    ~label=textualBody##value,
                    ~annotations,
                    ~agent,
                  )
                  ->Js.Option.some
                  ->Js.Promise.resolve;
                }
              : (() => Js.Promise.resolve(None));

          Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationsToCollection(
            ~annotations,
            ~annotationCollectionId,
            ~annotationCollectionLabel=textualBody##value,
            ~annotationCollectionType="TAG_COLLECTION",
            ~identityId,
            ~onCreateAnnotationCollection,
          );
        });
    };

    let _ = {
      let externalTargetAnnotationTuples =
        inputs
        ->Belt.Array.mapWithIndex((idx, input) => {
            let cacheAnnotation = cacheAnnotations->Belt.Array.getExn(idx);
            input##target
            ->Belt.Array.keepMap(target => {
                let externalTarget =
                  switch (
                    target##externalTarget,
                    target##specificTarget,
                    target##textualTarget,
                  ) {
                  | (Some(externalTarget), _, _) => Some(externalTarget)
                  | (_, Some(specificTarget), _) =>
                    switch (specificTarget##source##externalTarget) {
                    | Some(externalTarget) => Some(externalTarget)
                    | None => None
                    }
                  | _ => None
                  };
                externalTarget->Belt.Option.flatMap(externalTarget =>
                  switch (externalTarget##format) {
                  | Some(`TEXT_HTML) =>
                    Some((externalTarget, cacheAnnotation))
                  | _ => None
                  }
                );
              });
          })
        ->Belt.Array.concatMany;
      let externalTargetById =
        externalTargetAnnotationTuples->Belt.Array.reduce(
          Js.Dict.empty(),
          (agg, (externalTarget, _)) => {
            if (Js.Dict.get(agg, externalTarget##id)->Js.Option.isNone) {
              let _ = Js.Dict.set(agg, externalTarget##id, externalTarget);
              ();
            };
            agg;
          },
        );

      externalTargetAnnotationTuples
      ->Belt.Array.reduce(
          Js.Dict.empty(),
          (agg, (externalTarget, cacheAnnotation)) => {
            let annotations =
              agg
              ->Js.Dict.get(externalTarget##id)
              ->Belt.Option.map(a =>
                  Belt.Array.concat(a, [|cacheAnnotation|])
                )
              ->Belt.Option.getWithDefault([|cacheAnnotation|]);
            let _ = Js.Dict.set(agg, externalTarget##id, annotations);
            agg;
          },
        )
      ->Js.Dict.entries
      ->Belt.Array.forEach(((annotationCollectionId, annotations)) => {
          let externalTarget =
            Js.Dict.get(externalTargetById, annotationCollectionId)
            ->Belt.Option.getExn;
          let onCreateAnnotationCollection = () => Js.Promise.resolve(None);
          Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationsToCollection(
            ~annotations,
            ~annotationCollectionType="SOURCE_COLLECTION",
            ~annotationCollectionId=
              Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
                ~identityId,
                ~annotationCollectionIdComponent=externalTarget##hashId,
                (),
              ),
            ~annotationCollectionLabel=externalTarget##id,
            ~identityId,
            ~onCreateAnnotationCollection,
          );
        });
    };
    ();
  };
};
