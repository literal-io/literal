open Containers_NoteEditor_Notes_GraphQL;

let handleSave =
  Lodash.debounce3(
    (.
      variables,
      updateCache,
      updateHighlightMutation:
        ApolloHooks.Mutation.mutation(UpdateHighlightMutation.t),
    ) => {
      let _ = updateHighlightMutation(~variables, ());
      let _ = updateCache();
      ();
    },
    500,
  );

let handleUpdateCache =
    (
      ~highlight,
      ~editorValue: Containers_NoteEditor_Base.value,
      ~createHighlightTagsInput,
      ~currentUser,
      (),
    ) => {
  let cacheQuery =
    QueryRenderers_Notes_GraphQL.ListHighlights.Query.make(
      ~owner=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      (),
    );
  let highlightTags =
    highlight##tags
    ->Belt.Option.flatMap(t => t##items)
    ->Belt.Option.map(t => t->Belt.Array.keepMap(t => t))
    ->Belt.Option.getWithDefault([||]);
  let _ =
    switch (
      QueryRenderers_Notes_GraphQL.ListHighlights.readCache(
        ~query=cacheQuery,
        ~client=Provider.client,
        (),
      )
    ) {
    | None => ()
    | Some(cachedQuery) =>
      let updatedListHighlights =
        QueryRenderers_Notes_GraphQL.ListHighlights.Raw.(
          cachedQuery
          ->listHighlights
          ->Belt.Option.flatMap(highlightConnectionItems)
          ->Belt.Option.flatMap(items => {
              switch (
                items->Belt.Array.getIndexBy(
                  fun
                  | Some(h) when h.id === highlight##id => true
                  | _ => false,
                )
              ) {
              | Some(idx) => Some((idx, items))
              | None => None
              }
            })
          ->Belt.Option.map(((itemIdx, items)) => {
              let highlight =
                items
                ->Belt.Array.get(itemIdx)
                ->Belt.Option.flatMap(i => i)
                ->Belt.Option.getExn;

              let updatedTags =
                editorValue.tags
                ->Belt.Array.map(committedTag => {
                    switch (
                      highlightTags->Belt.Array.getBy(tag =>
                        tag##tag##id === committedTag##id
                      )
                    ) {
                    | Some(existingTag) =>
                      makeHighlightTag(
                        ~id=existingTag##id,
                        ~createdAt=existingTag##createdAt,
                        ~tag=
                          makeTag(
                            ~id=existingTag##tag##id,
                            ~text=existingTag##tag##text,
                          ),
                      )
                      ->Js.Option.some
                    | None =>
                      switch (
                        createHighlightTagsInput->Belt.Array.getBy(input =>
                          input##tagId === committedTag##id
                        )
                      ) {
                      | Some(input) =>
                        makeHighlightTag(
                          ~id=input##id->Js.Option.getExn,
                          ~createdAt=Js.Date.(make()->toISOString),
                          ~tag=
                            makeTag(
                              ~id=input##tagId,
                              ~text=committedTag##text,
                            ),
                        )
                        ->Js.Option.some
                      | None =>
                        let _ =
                          Error.(
                            report(
                              InvalidState(
                                "Expected highlight tag in cache or created, but found none.",
                              ),
                            )
                          );
                        None;
                      }
                    }
                  });
              let updatedHighlight = {
                ...highlight,
                text: editorValue.text,
                tags:
                  makeHighlightTagsConnection(
                    ~tags=updatedTags->Js.Option.some,
                  )
                  ->Js.Option.some,
              };
              let updatedHighlights =
                Belt.Array.concatMany([|
                  Belt.Array.slice(items, ~offset=0, ~len=itemIdx - 1),
                  [|Some(updatedHighlight)|],
                  Belt.Array.sliceToEnd(
                    items,
                    max(itemIdx + 1, Js.Array2.length(items) - 1),
                  ),
                |]);
              {
                ...cachedQuery,
                listHighlights:
                  Some(
                    makeHighlightConnection(
                      ~highlights=Some(updatedHighlights),
                    ),
                  ),
              };
            })
        );
      let _ =
        switch (updatedListHighlights) {
        | Some(updatedListHighlights) =>
          QueryRenderers_Notes_GraphQL.ListHighlights.writeCache(
            ~client=Provider.client,
            ~data=updatedListHighlights,
            ~query=cacheQuery,
            (),
          )
        | None => ()
        };
      ();
    };
  ();
};

[@react.component]
let make = (~highlightFragment as highlight, ~isActive, ~currentUser) => {
  let (updateHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(UpdateHighlightMutation.definition);

  let handleChange = (editorValue: Containers_NoteEditor_Base.value) => {
    let highlightTags =
      highlight##tags
      ->Belt.Option.flatMap(t => t##items)
      ->Belt.Option.map(t => t->Belt.Array.keepMap(t => t))
      ->Belt.Option.getWithDefault([||]);

    let (createTagsInput, createHighlightTagsInput) = {
      let tagsToCreate =
        editorValue.tags
        ->Belt.Array.keep(tag => {
            let alreadyExists =
              Belt.Array.some(highlightTags, highlightTag =>
                highlightTag##tag##id === tag##id
              );
            !alreadyExists;
          });
      /**
     * FIXME: this may recreate the tag if it isn't associated,
     * but already exists?
     */
      let createTagsInput =
        tagsToCreate->Belt.Array.map(tag =>
          {"id": tag##id, "text": tag##text, "createdAt": None}
        );
      let createHighlightTagsInput =
        createTagsInput->Belt.Array.map(tag =>
          {
            "id": Some(Uuid.makeV4()),
            "highlightId": highlight##id,
            "tagId": tag##id,
            "createdAt": None,
          }
        );
      (createTagsInput, createHighlightTagsInput);
    };

    let deleteHighlightTagsInput =
      highlightTags
      ->Belt.Array.keep(highlightTag => {
          let retained =
            Belt.Array.some(editorValue.tags, tag =>
              highlightTag##tag##id === tag##id
            );
          !retained;
        })
      ->Belt.Array.map(highlightTag => {"id": highlightTag##id});

    let variables =
      UpdateHighlightMutation.makeVariables(
        ~updateHighlightInput={
          "id": highlight##id,
          "text": editorValue.text->Js.Option.some,
          "createdAt": None,
          "note": None,
          "highlightScreenshotId": None,
          "owner": None,
        },
        ~createTagsInput,
        ~createHighlightTagsInput,
        ~deleteHighlightTagsInput,
        (),
      );
    let _ =
      handleSave(.
        variables,
        handleUpdateCache(
          ~highlight,
          ~editorValue,
          ~createHighlightTagsInput,
          ~currentUser,
        ),
        updateHighlightMutation,
      );
    ();
  };

  let _ =
    React.useEffect0(() => {
      Some(
        () => {
          let _ = Lodash.flush3(handleSave);
          ();
        },
      )
    });
  <Containers_NoteEditor_Base
    highlightFragment=highlight
    isActive
    onChange=handleChange
  />;
};
