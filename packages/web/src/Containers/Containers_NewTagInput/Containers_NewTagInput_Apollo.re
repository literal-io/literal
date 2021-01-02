let updateCacheForStaticTags = (~tags, ~currentUser, ~cacheAnnotation) =>
  tags->Belt.Array.forEach(staticTag => {
    let cacheQuery =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~id=staticTag##id->Js.Null.toOption->Belt.Option.getExn,
        (),
      );
    let data =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
        ~query=cacheQuery,
        ~client=Providers_Apollo.client,
        (),
      );
    /**
      * Only update collections we've already queried for, as the latest state will be
      * picked up in the rest on first query.
      */
    let _ =
      data
      ->Belt.Option.flatMap(data => {
          let items =
            data##getAnnotationCollection
            ->Js.Null.toOption
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.getWithDefault([||]);

          items
          ->Belt.Array.getIndexBy(a =>
              a##annotation##id === cacheAnnotation##id
            )
          ->Belt.Option.map(idx => {
              let newItems = Js.Array2.copy(items);
              let _ =
                Js.Array2.spliceInPlace(
                  newItems,
                  ~pos=idx,
                  ~remove=1,
                  ~add=[|{"__typename": "", "annotation": cacheAnnotation}|],
                );
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
                data,
                Js.Null.return(newItems),
              );
            });
        })
      ->Belt.Option.forEach(newData =>
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
            ~query=cacheQuery,
            ~client=Providers_Apollo.client,
            ~data=newData,
            (),
          )
        );
    ();
  });

let updateCacheForNewTag = (~tag, ~currentUser, ~cacheAnnotation) => {
  let cacheQuery =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
      ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      ~id=tag##id->Belt.Option.getExn,
      (),
    );
  let data =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
      ~query=cacheQuery,
      ~client=Providers_Apollo.client,
      (),
    );
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
            [|{"__typename": "", "annotation": cacheAnnotation}|],
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
};

let updateCache =
    (~annotation, ~currentUser, ~patchAnnotationMutationInput, ~tag) => {
  let cacheAnnotation =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.parsedAnnotationToCache(
      annotation,
    );
  let updatedCacheAnnotationBody =
    patchAnnotationMutationInput##operations
    ->Belt.Array.keepMap(op =>
        op##set
        ->Belt.Option.flatMap(op => op##body)
        ->Belt.Option.map(body =>
            body->Belt.Array.keepMap(b => {
              b##textualBody
              ->Belt.Option.flatMap(textualBody => {
                  let isTag =
                    textualBody##purpose
                    ->Belt.Option.map(p =>
                        p->Belt.Array.some(p => {
                          switch (p) {
                          | `TAGGING => true
                          | _ => false
                          }
                        })
                      )
                    ->Belt.Option.getWithDefault(false);

                  isTag ? Some(textualBody) : None;
                })
            })
          )
      )
    ->Belt.Array.concatMany
    ->Belt.Array.map(
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.parsedTextualBodyToCache,
      );

  let updatedCacheAnnotationModified =
    patchAnnotationMutationInput##operations
    ->Belt.Array.keepMap(op => op##set->Belt.Option.flatMap(s => s##modified))
    ->Belt.Array.get(0)
    ->Belt.Option.getWithDefault(
        Js.Date.(make()->toISOString)->Js.Json.string,
      );

  let updatedCacheAnnotation =
    Ramda.mergeDeepLeft(
      {
        "body": Some(updatedCacheAnnotationBody),
        "modified": Some(updatedCacheAnnotationModified),
      },
      cacheAnnotation,
    );

  let staticTags =
    updatedCacheAnnotationBody->Belt.Array.keep(body =>
      body##id->Js.Null.toOption != tag##id
    );

  let _ =
    updateCacheForStaticTags(
      ~cacheAnnotation=updatedCacheAnnotation,
      ~currentUser,
      ~tags=staticTags,
    );
  let _ =
    updateCacheForNewTag(
      ~cacheAnnotation=updatedCacheAnnotation,
      ~currentUser,
      ~tag,
    );
  ();
};
