module Apollo = {
  open QueryRenderers_AnnotationCollection_GraphQL;
  let updateCacheAnnotationCollectionItems =
      (~currentUser, ~annotationCollectionId, ~updateItems) => {
    let cacheQuery =
      GetAnnotationCollection.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~id=annotationCollectionId,
        (),
      );
    let _ =
      GetAnnotationCollection.readCache(
        ~query=cacheQuery,
        ~client=Providers_Apollo.client,
        (),
      )
      ->Belt.Option.flatMap(data => {
          let newItems =
            data##getAnnotationCollection
            ->Js.Null.toOption
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.getWithDefault([||])
            ->updateItems;

          newItems->Belt.Option.map(newItems => {
            GetAnnotationCollection.setAnnotationPageItems(
              data,
              Js.Null.return(newItems),
            )
          });
        })
      ->Belt.Option.forEach(newData =>
          GetAnnotationCollection.writeCache(
            ~query=cacheQuery,
            ~client=Providers_Apollo.client,
            ~data=newData,
            (),
          )
        );
    ();
  };

  let setAnnotationInCollection =
      (~annotation, ~currentUser, ~annotationCollectionId) =>
    updateCacheAnnotationCollectionItems(
      ~currentUser, ~annotationCollectionId, ~updateItems=items => {
      items
      ->Belt.Array.getIndexBy(a => a##annotation##id === annotation##id)
      ->Belt.Option.map(idx => {
          let newItems = Js.Array2.copy(items);
          let _ =
            Js.Array2.spliceInPlace(
              newItems,
              ~pos=idx,
              ~remove=1,
              ~add=[|{"__typename": "", "annotation": annotation}|],
            );
          newItems;
        })
    });

  let addAnnotationToCollection =
      (~annotation, ~currentUser, ~annotationCollectionId) =>
    updateCacheAnnotationCollectionItems(
      ~currentUser, ~annotationCollectionId, ~updateItems=items => {
      Belt.Array.concat(
        [|{"__typename": "", "annotation": annotation}|],
        items,
      )
      ->Js.Option.some
    });

  let addAnnotationsToCollection =
      (~annotations, ~currentUser, ~annotationCollectionId) =>
    updateCacheAnnotationCollectionItems(
      ~currentUser, ~annotationCollectionId, ~updateItems=items =>
      annotations
      ->Belt.Array.map(annotation =>
          {"__typename": "", "annotation": annotation}
        )
      ->Belt.Array.concat(items)
      ->Js.Option.some
    );

  let removeAnnotationFromCollection =
      (~annotationId, ~currentUser, ~annotationCollectionId) =>
    updateCacheAnnotationCollectionItems(
      ~currentUser, ~annotationCollectionId, ~updateItems=items => {
      items
      ->Belt.Array.keep(d => d##annotation##id != annotationId)
      ->Js.Option.some
    });
};
