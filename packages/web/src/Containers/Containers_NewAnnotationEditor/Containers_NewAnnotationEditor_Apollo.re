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

        let _ =
          data->Belt.Option.forEach(data => {
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
            let newData =
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
                data,
                newItems,
              );
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
      });
  ();
};
