let updateCacheForStaticTags = (~tags, ~currentUser, ~cacheAnnotation) =>
  tags->Belt.Array.forEach(staticTag => {
    let cacheQuery =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~id=staticTag##id->Belt.Option.getExn,
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

let updateCacheForAddedTags =
    (
      ~tags: array(Containers_AnnotationEditor_Types.tag),
      ~currentUser,
      ~cacheAnnotation,
    ) =>
  tags->Belt.Array.forEach(addedTag => {
    let cacheQuery =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~id=addedTag.id->Belt.Option.getExn,
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
  });

let updateCacheForRemovedTags = (~tags, ~currentUser, ~annotation) =>
  tags->Belt.Array.forEach(removedTag => {
    let cacheQuery =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~id=removedTag##id->Belt.Option.getExn,
        (),
      );
    let data =
      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
        ~query=cacheQuery,
        ~client=Providers_Apollo.client,
        (),
      );
    let _ =
      data
      ->Belt.Option.flatMap(d => d##getAnnotationCollection->Js.Null.toOption)
      ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
      ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
      ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
      ->Belt.Option.forEach(items => {
          let newItems =
            items
            ->Belt.Array.keep(d => d##annotation##id != annotation##id)
            ->Js.Null.return;
          let newData =
            QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
              data->Js.Option.getExn,
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

let updateCache =
    (
      ~annotation,
      ~tags: array(Containers_AnnotationEditor_Types.tag),
      ~currentUser,
      ~patchAnnotationMutationInput,
    ) => {
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

  let updatedCacheAnnotation =
    Ramda.mergeDeepLeft(
      {"body": Some(updatedCacheAnnotationBody)},
      cacheAnnotation,
    );

  let currentTags =
    annotation##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
          Some(body)
        | _ => None
        }
      );
  let addedTags =
    tags->Belt.Array.keep(tag =>
      currentTags
      ->Belt.Array.getBy(currentTag => currentTag##id == tag.id)
      ->Js.Option.isNone
    );
  let removedTags =
    currentTags->Belt.Array.keep(currentTag =>
      tags->Belt.Array.getBy(tag => tag.id == currentTag##id)->Js.Option.isNone
    );

  let staticTags =
    currentTags->Belt.Array.keep(currentTag =>
      tags->Belt.Array.getBy(tag => tag.id == currentTag##id)->Js.Option.isSome
    );

  let _ =
    updateCacheForStaticTags(
      ~tags=staticTags,
      ~currentUser,
      ~cacheAnnotation=updatedCacheAnnotation,
    );
  let _ =
    updateCacheForRemovedTags(~tags=removedTags, ~currentUser, ~annotation);
  let _ =
    updateCacheForAddedTags(
      ~tags=addedTags,
      ~currentUser,
      ~cacheAnnotation=updatedCacheAnnotation,
    );
  ();
};
