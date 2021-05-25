let unsafeAsCache = [%raw
  {|
  function asCache(annotation) {

    const stateAsCache = (state) => {
      if (state.__typename === "TimeState") {
        return {
          ...state,
          type_: state.type
        }
      }

      return state
    }

    const selectorAsCache = (selector) => {
      if (selector.__typename === "RangeSelector") {
        return {
          ...selector,
          startSelector: selectorAsCache(selector.startSelector),
          endSelector: selectorAsCache(selector.endSelector),
          type_: selector.type
        }
      } else if (selector.__typename === "XPathSelector") {
        return {
          ...selector,
          refinedBy: selector.refinedBy
            ? selector.refinedBy.map(selectorAsCache)
            : null,
          type_: selector.type
        }
      } else if (selector.__typename === "TextPositionSelector") {
        return {
          ...selector,
          end_: selector.end,
          type_: selector.type
        }
      }

      return selector
    }

    const targetAsCache = (target) => {
      if (target.__typename === "SpecificTarget") {
        return {
          ...target,
          specificTargetId: target.id,
          selector:
            target.selector
              ? target.selector.map(selectorAsCache)
              : null,
          state:
            target.state
              ? target.state.map(stateAsCache)
              : null,
          source: targetAsCache(target.source)
        }
      } else if (target.__typename === "ExternalTarget") {
        return {
          ...target,
          externalTargetId: target.id,
          type_: target.type
        }
      } else if (target.__typename === "TextualTarget") {
        return {
          ...target,
          textualTargetId: target.id
        }
      }
      return target
    }

    return {
      ...annotation,
      target:
        annotation.target
          ? annotation.target.map(targetAsCache)
          : null
    }
  }
|}
];

let writeToCache = (~annotation, ~identityId) => {
  let cacheAnnotation =
    annotation->LiteralModel.Annotation.encode->unsafeAsCache;

  // FIXME: handle AnnotationCollectionsDrawer query, need to diff against cache
  // to determine what values have changed

  annotation.body
  ->Belt.Option.getWithDefault([||])
  ->Belt.Array.keepMap(body =>
      switch (body) {
      | LiteralModel.Body.TextualBody(textualBody)
          when
            textualBody.purpose
            ->Belt.Option.map(a =>
                a->Belt.Array.some(purpose => purpose == "TAGGING")
              )
            ->Belt.Option.getWithDefault(false) =>
        Some(textualBody.id)
      | _ => None
      }
    )
  ->Belt.Array.forEach(annotationCollectionId => {
      Lib_GraphQL_AnnotationCollection.Apollo.setAnnotationInCollection(
        ~annotation=cacheAnnotation,
        ~identityId,
        ~annotationCollectionId,
      )
    });
};

let addManyToCache = (~annotations, ~identityId) => {
  let _ = {
    let textualBodyAnnotationTuples =
      annotations
      ->Belt.Array.map(annotation => {
          let cacheAnnotation =
            annotation->LiteralModel.Annotation.encode->unsafeAsCache;

          annotation.body
          ->Belt.Option.getWithDefault([||])
          ->Belt.Array.keepMap(body =>
              switch (body) {
              | TextualBody(body)
                  when
                    body.purpose
                    ->Belt.Option.map(a =>
                        a->Belt.Array.some(purpose => purpose == "TAGGING")
                      )
                    ->Belt.Option.getWithDefault(false) =>
                Some((body, cacheAnnotation))
              | _ => None
              }
            );
        })
      ->Belt.Array.concatMany;

    textualBodyAnnotationTuples
    ->Belt.Array.reduce(
        Js.Dict.empty(),
        (agg, (textualBody, cacheAnnotation)) => {
          let update =
            agg
            ->Js.Dict.get(textualBody.id)
            ->Belt.Option.map(((annotations, textualBody)) =>
                (
                  Belt.Array.concat(annotations, [|cacheAnnotation|]),
                  textualBody,
                )
              )
            ->Belt.Option.getWithDefault(([|cacheAnnotation|], textualBody));

          let _ = Js.Dict.set(agg, textualBody.id, update);
          agg;
        },
      )
    ->Js.Dict.entries
    ->Belt.Array.forEach(
        ((annotationCollectionId, (annotations, textualBody))) => {
        let onCreateAnnotationCollection = () => Js.Promise.resolve(None);
        Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationsToCollection(
          ~annotations,
          ~annotationCollectionId,
          ~annotationCollectionLabel=textualBody.value,
          ~annotationCollectionType="TAG_COLLECTION",
          ~identityId,
          ~onCreateAnnotationCollection,
        );
      });
  };

  let _ = {
    let externalTargetAnnotationTuples =
      annotations
      ->Belt.Array.map(annotation => {
          let cacheAnnotation =
            annotation->LiteralModel.Annotation.encode->unsafeAsCache;

          annotation.target
          ->Belt.Array.keepMap(target => {
              switch (target) {
              | ExternalTarget(target) => Some((target, cacheAnnotation))
              | SpecificTarget({
                  source: ExternalTarget({format: Some(format)} as target),
                })
                  when format == "TEXT_HTML" =>
                Some((target, cacheAnnotation))
              | _ => None
              }
            });
        })
      ->Belt.Array.concatMany;

    let externalTargetById =
      externalTargetAnnotationTuples->Belt.Array.reduce(
        Js.Dict.empty(),
        (agg, (externalTarget, _)) => {
          if (Js.Dict.get(agg, externalTarget.id)->Js.Option.isNone) {
            let _ = Js.Dict.set(agg, externalTarget.id, externalTarget);
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
            ->Js.Dict.get(externalTarget.id)
            ->Belt.Option.map(a => Belt.Array.concat(a, [|cacheAnnotation|]))
            ->Belt.Option.getWithDefault([|cacheAnnotation|]);
          let _ = Js.Dict.set(agg, externalTarget.id, annotations);
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
              ~annotationCollectionIdComponent=
                externalTarget.hashId->Belt.Option.getWithDefault(""),
              (),
            ),
          ~annotationCollectionLabel=externalTarget.id,
          ~identityId,
          ~onCreateAnnotationCollection,
        );
      });
  };

  ();
};

let deleteFromCache = (~annotation, ~identityId) => {
  let tagAnnotationCollectionIds =
    annotation.LiteralModel.Annotation.body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        switch (body) {
        | LiteralModel.Body.TextualBody(textualBody)
            when
              textualBody.purpose
              ->Belt.Option.map(a =>
                  a->Belt.Array.some(purpose => purpose == "TAGGING")
                )
              ->Belt.Option.getWithDefault(false) =>
          Some(textualBody.id)
        | _ => None
        }
      )
    ->Belt.Array.map(Js.Promise.resolve);

  let sourceAnnotationCollectionIds =
    annotation.target
    ->Belt.Array.keepMap(target => {
        switch (target) {
        | ExternalTarget(target) => Some(target.id)
        | SpecificTarget({
            source: ExternalTarget({format: Some(format)} as target),
          })
            when format == "TEXT_HTML" =>
          Some(target.id)
        | _ => None
        }
      })
    ->Belt.Array.map(id =>
        Lib_GraphQL.AnnotationCollection.makeId(~identityId, ~label=id)
      );

  let _ =
    Belt.Array.concat(
      tagAnnotationCollectionIds,
      sourceAnnotationCollectionIds,
    )
    |> Js.Promise.all
    |> Js.Promise.then_(annotationCollectionIds => {
         let _ =
           annotationCollectionIds->Belt.Array.forEach(annotationCollectionId =>
             annotation.id
             ->Belt.Option.forEach(annotationId =>
                 Lib_GraphQL_AnnotationCollection.Apollo.removeAnnotationFromCollection(
                   ~annotationId,
                   ~identityId,
                   ~annotationCollectionId,
                 )
               )
           );
         Js.Promise.resolve();
       });
  ();
};
