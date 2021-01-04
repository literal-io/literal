open Containers_Onboarding_GraphQL;

let updateCache = (~currentUser, ~createAnnotationInputs, ~createAgentInput) => {
  let tagAnnotationTuples =
    createAnnotationInputs
    ->Belt.Array.reverse
    ->Belt.Array.map(annotationInput =>
        annotationInput
        ->OnboardingMutation.json_of_CreateAnnotationInput
        ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput
      )
    ->Belt.Array.map(annotation =>
        annotation##body
        ->Js.Null.toOption
        ->Belt.Option.getWithDefault([||])
        ->Belt.Array.keepMap(body => {
            let isTag =
              body##purpose
              ->Js.Null.toOption
              ->Belt.Option.map(d => d->Belt.Array.some(p => p == "TAGGING"))
              ->Belt.Option.getWithDefault(false)
              &&
              body##__typename == "TextualBody"
              && body##id->Js.Null.toOption->Belt.Option.isSome;
            isTag ? Some((annotation, body)) : None;
          })
      )
    ->Belt.Array.concatMany;

  let annotationsByTagId =
    tagAnnotationTuples->Belt.Array.reduce(
      Js.Dict.empty(),
      (agg, (annotation, tag)) => {
        let tagId = tag##id->Js.Null.toOption->Belt.Option.getExn;
        let annotations =
          agg
          ->Js.Dict.get(tagId)
          ->Belt.Option.map(a => Belt.Array.concat(a, [|annotation|]))
          ->Belt.Option.getWithDefault([|annotation|]);
        let _ = Js.Dict.set(agg, tagId, annotations);
        agg;
      },
    );
  let tagsByTagId =
    tagAnnotationTuples
    ->Belt.Array.map(((_, tag)) => tag)
    ->Belt.Array.reduce(
        Js.Dict.empty(),
        (agg, tag) => {
          let tagId = tag##id->Js.Null.toOption->Belt.Option.getExn;
          let _ = Js.Dict.set(agg, tagId, tag);
          agg;
        },
      );

  annotationsByTagId
  ->Js.Dict.entries
  ->Belt.Array.forEach(((tagId, annotations)) => {
      let cacheQuery =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
          ~creatorUsername=
            currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
          ~id=tagId,
          (),
        );
      let data =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
          ~query=cacheQuery,
          ~client=Providers_Apollo.client,
          (),
        );
      let tag = tagsByTagId->Js.Dict.unsafeGet(tagId);
      let newData =
        switch (data) {
        | Some(data) when data##getAnnotationCollection != Js.null =>
          let items =
            data##getAnnotationCollection
            ->Js.Null.toOption
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.getWithDefault([||]);
          let newItems =
            annotations
            ->Belt.Array.map(a =>
                {"__typename": "AnnotationPageItem", "annotation": a}
              )
            ->Belt.Array.concat(items)
            ->Js.Null.return;

          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
            data,
            newItems,
          );
        | _ => {
            "__typename": "Query",
            "getAgent":
              Js.Null.return({
                "__typename": "Agent",
                "id": createAgentInput##id,
              }),
            "getAnnotationCollection":
              Js.Null.return({
                "__typename": "AnnotationCollection",
                "label": tag##value,
                "first":
                  Js.Null.return({
                    "__typename": "AnnotationPage",
                    "items":
                      Js.Null.return({
                        "__typename": "ModelAnnotationPageItemConnection",
                        "nextToken": Js.Null.empty,
                        "items":
                          annotations
                          ->Belt.Array.map(a =>
                              {
                                "__typename": "AnnotationPageItem",
                                "annotation": a,
                              }
                            )
                          ->Js.Null.return,
                      }),
                  }),
              }),
          }
        };
      let _ =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
          ~query=cacheQuery,
          ~client=Providers_Apollo.client,
          ~data=newData,
          (),
        );
      ();
    });
  ();
};
