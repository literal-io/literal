module Apollo = {
  open QueryRenderers_AnnotationCollection_GraphQL;

  let updateCacheAnnotationCollectionItems =
      (
        ~currentUser,
        ~annotationCollectionId,
        ~onUpdateItems,
        ~onCreateAnnotationCollection,
      ) => {
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
          switch (data##getAnnotationCollection->Js.Null.toOption) {
          | Some(getAnnotationCollection) =>
            let newItems =
              getAnnotationCollection##first
              ->Js.Null.toOption
              ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
              ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
              ->Belt.Option.getWithDefault([||])
              ->onUpdateItems;

            newItems->Belt.Option.map(newItems => {
              GetAnnotationCollection.setAnnotationPageItems(
                data,
                Js.Null.return(newItems),
              )
            });
          | None => onCreateAnnotationCollection()
          }
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
      (~annotation, ~currentUser, ~annotationCollectionId) => {
    let onUpdateItems = items =>
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
        });
    let onCreateAnnotationCollection = () => None;

    updateCacheAnnotationCollectionItems(
      ~currentUser,
      ~annotationCollectionId,
      ~onUpdateItems,
      ~onCreateAnnotationCollection,
    );
  };

  let addAnnotationToCollection =
      (
        ~annotation,
        ~currentUser,
        ~annotationCollectionId,
        ~onCreateAnnotationCollection,
      ) => {
    let onUpdateItems = items =>
      Belt.Array.concat(
        [|{"__typename": "", "annotation": annotation}|],
        items,
      )
      ->Js.Option.some;

    updateCacheAnnotationCollectionItems(
      ~currentUser,
      ~annotationCollectionId,
      ~onUpdateItems,
      ~onCreateAnnotationCollection,
    );
  };

  let addAnnotationsToCollection =
      (
        ~annotations,
        ~currentUser,
        ~annotationCollectionId,
        ~onCreateAnnotationCollection,
      ) => {
    let onUpdateItems = items =>
      annotations
      ->Belt.Array.map(annotation =>
          {"__typename": "", "annotation": annotation}
        )
      ->Belt.Array.concat(items)
      ->Js.Option.some;

    updateCacheAnnotationCollectionItems(
      ~currentUser,
      ~annotationCollectionId,
      ~onUpdateItems,
      ~onCreateAnnotationCollection,
    );
  };

  let removeAnnotationFromCollection =
      (~annotationId, ~currentUser, ~annotationCollectionId) => {
    let onUpdateItems = items =>
      items
      ->Belt.Array.keep(d => d##annotation##id != annotationId)
      ->Js.Option.some;

    let onCreateAnnotationCollection = () => None;

    updateCacheAnnotationCollectionItems(
      ~currentUser,
      ~annotationCollectionId,
      ~onUpdateItems,
      ~onCreateAnnotationCollection,
    );
  };
};
