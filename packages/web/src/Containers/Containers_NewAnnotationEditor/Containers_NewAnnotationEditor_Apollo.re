let updateCache = (~currentUser, ~input) => {
  let newAnnotation =
    input
    ->Containers_NewAnnotationEditor_GraphQL.CreateAnnotationMutation.json_of_CreateAnnotationInput
    ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput;

  let _ =
    newAnnotation##body
    ->Belt.Option.map(bodies =>
        bodies->Belt.Array.keep(body =>
          body##purpose
          ->Js.Null.toOption
          ->Belt.Option.map(d => d->Belt.Array.some(p => p === "TAGGING"))
          ->Belt.Option.getWithDefault(false)
          &&
          body##__typename == "TextualBody"
          && body##id->Js.Null.toOption->Belt.Option.isSome
        )
      )
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.forEach(tag => {
        let cacheQuery =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=tag##id->Js.Null.toOption->Belt.Option.getExn,
            (),
          );
        let data =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
            ~query=cacheQuery,
            ~client=Providers_Apollo.client,
            (),
          );
        let newAnnotation =
          input
          ->Containers_NewAnnotationEditor_GraphQL.CreateAnnotationMutation.json_of_CreateAnnotationInput
          ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput;

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
              Js.Null.return(
                Belt.Array.concat(
                  [|
                    {
                      "__typename": "AnnotationPageItem",
                      "annotation": newAnnotation,
                    },
                  |],
                  items,
                ),
              );
            Some(
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
                data,
                newItems,
              ),
            );
          | Some(data) when data##getAnnotationCollection == Js.null =>
            let newData = {
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
                        Js.Null.return([|
                          {
                            "__typename": "AnnotationPageItem",
                            "annotation": newAnnotation,
                          },
                        |]),
                    }),
                }),
            };
            Some(
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setGetAnnotationCollection(
                data,
                newData,
              ),
            );
          | _ => None
          };

        let _ =
          newData->Belt.Option.forEach(newData => {
            QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
              ~query=cacheQuery,
              ~client=Providers_Apollo.client,
              ~data=newData,
              (),
            )
          });
        ();
      });
  ();
};
