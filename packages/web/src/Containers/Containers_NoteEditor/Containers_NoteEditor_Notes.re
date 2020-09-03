open Containers_NoteEditor_Notes_GraphQL;

let handleSave =
  Lodash.debounce2(
    (.
      variables,
      /**updateCache,**/ updateAnnotationMutation:
        ApolloHooks.Mutation.mutation(PatchAnnotationMutation.t),
    ) => {
      let _ = updateAnnotationMutation(~variables, ());
      /**let _ = updateCache();**/ ();
    },
    500,
  );

/**
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
                  Belt.Array.slice(items, ~offset=0, ~len=itemIdx),
                  [|Some(updatedHighlight)|],
                  itemIdx === Js.Array2.length(items) - 1
                    ? [||]
                    : Belt.Array.sliceToEnd(
                        items,
                        min(itemIdx + 1, Js.Array2.length(items) - 1),
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

**/

[@react.component]
let make = (~annotationFragment as annotation, ~isActive, ~currentUser) => {
  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(PatchAnnotationMutation.definition);

  let handleChange = (editorValue: Containers_NoteEditor_Base_Types.value) => {
    let updateTargetInput = {
      let idx =
        annotation##target
        ->Belt.Array.getIndexBy(target =>
            switch (target) {
            | `TextualTarget(_) => true
            | `ExternalTarget(_) => false
            }
          );
      let updatedTextualTarget =
        idx
        ->Belt.Option.flatMap(idx => annotation##target->Belt.Array.get(idx))
        ->Belt.Option.flatMap(target =>
            switch (target) {
            | `TextualTarget(target) =>
              let copy = Js.Obj.assign(Js.Obj.empty(), target);
              Some(
                `TextualTarget(
                  Js.Obj.assign(copy, {"value": editorValue.text}),
                ),
              );
            | `ExternalTarget(_) => None
            }
          )
        ->Belt.Option.getWithDefault(
            `TextualTarget({
              "__typename": "TextualTarget",
              "id": None,
              "format": Some(`TEXT_PLAIN),
              "language": Some(`EN_US),
              "processingLanguage": Some(`EN_US),
              "textDirection": Some(`LTR),
              "accessibility": None,
              "rights": None,
              "value": editorValue.text
            }),
          );

      let updatedTarget = Belt.Array.copy(annotation##target);
      let _ =
        switch (idx) {
        | Some(idx) =>
          let _ = updatedTarget->Belt.Array.set(idx, updatedTextualTarget);
          ();
        | None =>
          let _ = updatedTarget->Js.Array2.push(updatedTextualTarget);
          ();
        };

      updatedTarget->Belt.Array.map(
        Lib_GraphQL.Annotation.targetInputFromTarget,
      );
    };
    let updateBodyInput = {
      editorValue.tags
      ->Belt.Array.map(tag => {
          let id =
            switch (tag.id) {
            | Some(id) => Js.Promise.resolve(id)
            | None =>
              Lib_GraphQL.AnnotationCollection.makeId(
                ~creatorUsername=
                  AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                ~label=tag.text,
              )
            };
          id
          |> Js.Promise.then_(id =>
               Js.Promise.resolve({
                 "textualBody":
                   Some({
                     "id": Some(id),
                     "value": tag.text,
                     "purpose": Some([|`TAGGING|]),
                     "rights": None,
                     "accessibility": None,
                     "format": Some(`TEXT_PLAIN),
                     "textDirection": Some(`LTR),
                     "language": Some(`EN_US),
                     "processingLanguage": Some(`EN_US),
                     "type": Some(`TEXTUAL_BODY),
                   }),
                 "externalBody": None,
                 "choiceBody": None,
                 "specificBody": None,
               })
             );
        });
    };

    let _ =
      Js.Promise.all(updateBodyInput)
      |> Js.Promise.then_(updateBodyInput => {
           let variables =
             PatchAnnotationMutation.makeVariables(
               ~input={
                 "id": annotation##id,
                 "creatorUsername":
                   AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                 "operations": [|
                   {
                     "set":
                       Some({"body": Some(updateBodyInput), "target": None}),
                   },
                   {
                     "set":
                       Some({
                         "body": None,
                         "target": Some(updateTargetInput),
                       }),
                   },
                 |],
               },
               (),
             );
           let _ = handleSave(. variables, patchAnnotationMutation);
           Js.Promise.resolve();
         });
    ();
  };

  let _ =
    React.useEffect0(() => {
      Some(
        () => {
          let _ = Lodash.flush2(handleSave);
          ();
        },
      )
    });
  <Containers_NoteEditor_Base
    annotationFragment=annotation
    isActive
    onChange=handleChange
    currentUser
  />;
};
