module Apollo = {
  open QueryRenderers_AnnotationCollection_GraphQL;

  let updateCacheListAnnotationCollectionsItems =
      (~identityId, ~onUpdateItems, ~onCreateAnnotationCollections) => {
    let cacheQuery =
      QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.Query.make(
        ~creatorUsername=identityId,
        (),
      );

    let _ =
      QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.Cache.readCache(
        ~query=cacheQuery,
        ~client=Providers_Apollo_Client.inst^,
        (),
      )
      ->Belt.Option.forEach(data => {
          let newData =
            switch (data##listAnnotationCollections->Js.Null.toOption) {
            | Some(listAnnotationCollections) =>
              let newItems =
                listAnnotationCollections##items
                ->Js.Null.toOption
                ->Belt.Option.getWithDefault([||])
                ->onUpdateItems;

              newItems->Belt.Option.map(newItems => {
                QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.Cache.setItems(
                  data,
                  Js.Null.return(newItems),
                )
              });
            | None => onCreateAnnotationCollections()
            };

          let _ =
            newData->Belt.Option.forEach(newData =>
              QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.Cache.writeCache(
                ~query=cacheQuery,
                ~client=Providers_Apollo_Client.inst^,
                ~data=newData,
                (),
              )
            );
          ();
        });
    ();
  };

  let updateCacheAnnotationCollectionItems =
      (
        ~identityId,
        ~annotationCollectionId,
        ~onUpdateItems,
        ~onCreateAnnotationCollection,
      ) => {
    let cacheQuery =
      GetAnnotationCollection.Query.make(
        ~creatorUsername=identityId,
        ~id=annotationCollectionId,
        (),
      );
    let _ =
      GetAnnotationCollection.Cache.readCache(
        ~query=cacheQuery,
        ~client=Providers_Apollo_Client.inst^,
        (),
      )
      ->Belt.Option.forEach(data => {
          let newData =
            switch (data##getAnnotationCollection->Js.Null.toOption) {
            | Some(getAnnotationCollection) =>
              let newItems =
                getAnnotationCollection##first
                ->Js.Null.toOption
                ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
                ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
                ->Belt.Option.getWithDefault([||])
                ->onUpdateItems;

              newItems
              ->Belt.Option.map(newItems => {
                  GetAnnotationCollection.setAnnotationPageItems(
                    data,
                    Js.Null.return(newItems),
                  )
                })
              ->Js.Promise.resolve;
            | None => onCreateAnnotationCollection()
            };

          let _ =
            newData
            |> Js.Promise.then_(newData =>
                 newData
                 ->Belt.Option.forEach(newData =>
                     GetAnnotationCollection.Cache.writeCache(
                       ~query=cacheQuery,
                       ~client=Providers_Apollo_Client.inst^,
                       ~data=newData,
                       (),
                     )
                   )
                 ->Js.Promise.resolve
               );
          ();
        });
    ();
  };

  let setAnnotationInCollection =
      (~annotation, ~identityId, ~annotationCollectionId) => {
    let annotationId = a =>
      a
      ->Js.Json.decodeObject
      ->Belt.Option.flatMap(a => a->Js.Dict.get("id"))
      ->Belt.Option.flatMap(Js.Json.decodeString);

    let onUpdateItems = items =>
      items
      ->Belt.Array.getIndexBy(a =>
          switch (annotationId(a##annotation), annotationId(annotation)) {
          | (Some(id1), Some(id2)) when id1 == id2 => true
          | _ => false
          }
        )
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
    let onCreateAnnotationCollection = () => Js.Promise.resolve(None);

    updateCacheAnnotationCollectionItems(
      ~identityId,
      ~annotationCollectionId,
      ~onUpdateItems,
      ~onCreateAnnotationCollection,
    );
  };

  let addAnnotationToCollection =
      (
        ~annotation,
        ~identityId,
        ~annotationCollectionId,
        ~annotationCollectionLabel,
        ~annotationCollectionType,
        ~onCreateAnnotationCollection,
      ) => {
    let _ = {
      let onUpdateItems = items =>
        Belt.Array.concat(
          [|{"__typename": "", "annotation": annotation}|],
          items,
        )
        ->Js.Option.some;
      updateCacheAnnotationCollectionItems(
        ~identityId,
        ~annotationCollectionId,
        ~onUpdateItems,
        ~onCreateAnnotationCollection,
      );
    };

    let _ = {
      let onUpdateItems = items =>
        switch (
          Belt.Array.getIndexBy(items, item =>
            item##id == annotationCollectionId
          )
        ) {
        | Some(idx) =>
          let item = Belt.Array.getExn(items, idx);
          let newItem =
            QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCacheItem(
              ~type_=item##type_,
              ~id=item##id,
              ~label=item##label,
              ~total=item##total + 1,
            );
          let newItems = Js.Array2.copy(items);
          let _ =
            Js.Array2.spliceInPlace(
              newItems,
              ~pos=idx,
              ~remove=1,
              ~add=[|newItem|],
            );
          newItems->Js.Option.some;
        | None =>
          let newItem =
            QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCacheItem(
              ~type_=[|"ANNOTATION_COLLECTION", annotationCollectionType|],
              ~id=annotationCollectionId,
              ~label=annotationCollectionLabel,
              ~total=1,
            );
          Belt.Array.concat([|newItem|], items)->Js.Option.some;
        };

      let onCreateAnnotationCollections = () =>
        QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCache(
          ~items=onUpdateItems([||])->Belt.Option.getWithDefault([||]),
        )
        ->Js.Option.some;

      updateCacheListAnnotationCollectionsItems(
        ~identityId,
        ~onUpdateItems,
        ~onCreateAnnotationCollections,
      );
    };
    ();
  };

  let addAnnotationsToCollection =
      (
        ~annotations,
        ~identityId,
        ~annotationCollectionId,
        ~annotationCollectionLabel,
        ~annotationCollectionType,
        ~onCreateAnnotationCollection,
      ) => {
    let _ = {
      let onUpdateItems = items =>
        annotations
        ->Belt.Array.map(annotation =>
            {"__typename": "", "annotation": annotation}
          )
        ->Belt.Array.concat(items)
        ->Ramda.uniqBy(d =>
            d##annotation
            ->Js.Json.decodeObject
            ->Belt.Option.flatMap(a => a->Js.Dict.get("id"))
          )
        ->Js.Option.some;

      updateCacheAnnotationCollectionItems(
        ~identityId,
        ~annotationCollectionId,
        ~onUpdateItems,
        ~onCreateAnnotationCollection,
      );
    };

    let _ = {
      let onUpdateItems = items =>
        switch (
          Belt.Array.getIndexBy(items, item =>
            item##id == annotationCollectionId
          )
        ) {
        | Some(idx) =>
          let item = Belt.Array.getExn(items, idx);
          let newItem =
            QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCacheItem(
              ~type_=item##type_,
              ~id=item##id,
              ~label=item##label,
              ~total=item##total + Js.Array2.length(annotations),
            );
          let newItems = Js.Array2.copy(items);
          let _ =
            Js.Array2.spliceInPlace(
              newItems,
              ~pos=idx,
              ~remove=1,
              ~add=[|newItem|],
            );
          newItems->Js.Option.some;
        | None =>
          let newItem =
            QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCacheItem(
              ~type_=[|"ANNOTATION_COLLECTION", annotationCollectionType|],
              ~id=annotationCollectionId,
              ~label=annotationCollectionLabel,
              ~total=Js.Array2.length(annotations),
            );
          Belt.Array.concat([|newItem|], items)->Js.Option.some;
        };

      let onCreateAnnotationCollections = () =>
        QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCache(
          ~items=onUpdateItems([||])->Belt.Option.getWithDefault([||]),
        )
        ->Js.Option.some;

      updateCacheListAnnotationCollectionsItems(
        ~identityId,
        ~onUpdateItems,
        ~onCreateAnnotationCollections,
      );
    };
    ();
  };

  let removeAnnotationFromCollection =
      (~annotationId, ~identityId, ~annotationCollectionId) => {
    let _ = {
      let onUpdateItems = items =>
        items
        ->Belt.Array.keep(d =>
            d##annotation
            ->Js.Json.decodeObject
            ->Belt.Option.flatMap(a => a->Js.Dict.get("id"))
            ->Belt.Option.flatMap(Js.Json.decodeString)
            ->Belt.Option.map(id => id != annotationId)
            ->Belt.Option.getWithDefault(true)
          )
        ->Js.Option.some;

      let onCreateAnnotationCollection = () => Js.Promise.resolve(None);

      updateCacheAnnotationCollectionItems(
        ~identityId,
        ~annotationCollectionId,
        ~onUpdateItems,
        ~onCreateAnnotationCollection,
      );
    };

    let _ = {
      let onUpdateItems = items =>
        Belt.Array.getIndexBy(items, item =>
          item##id == annotationCollectionId
        )
        ->Belt.Option.map(idx => {
            let item = Belt.Array.getExn(items, idx);
            if (item##total > 1) {
              let newItem =
                QueryRenderers_AnnotationCollections_GraphQL.ListAnnotationCollections.CacheConfig.makeCacheItem(
                  ~type_=item##type_,
                  ~id=item##id,
                  ~label=item##label,
                  ~total=item##total - 1,
                );
              let newItems = Js.Array2.copy(items);
              let _ =
                Js.Array2.spliceInPlace(
                  newItems,
                  ~pos=idx,
                  ~remove=1,
                  ~add=[|newItem|],
                );
              Some(newItems);
            } else {
              let newItems = Js.Array2.copy(items);
              let _ =
                Js.Array2.spliceInPlace(
                  newItems,
                  ~pos=idx,
                  ~remove=1,
                  ~add=[||],
                );
              Some(newItems);
            };
          })
        ->Belt.Option.getWithDefault(Some(items));
      let onCreateAnnotationCollections = () => None;

      updateCacheListAnnotationCollectionsItems(
        ~identityId,
        ~onUpdateItems,
        ~onCreateAnnotationCollections,
      );
    };
    ();
  };

  let readCache = (~id, ~fragment) =>
    Apollo.Client.readFragment(
      Providers_Apollo_Client.inst^,
      {id: "AnnotationCollection:" ++ id, fragment},
    );
};
