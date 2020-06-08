open Styles;
open Containers_NoteEditor_GraphQL;
let styles = [%raw "require('./Containers_NoteEditor.module.css')"];

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

type tagState = {
  commits:
    array({
      .
      "id": string,
      "text": string,
    }),
  partial: string,
};

[@react.component]
let make = (~highlightFragment as highlight, ~isActive, ~currentUser) => {
  let (updateHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(UpdateHighlightMutation.definition);

  let (textState, setTextState) = React.useState(() => {highlight##text});
  let (tagsState, setTagsState) =
    React.useState(() =>
      highlight##tags
      ->Belt.Option.flatMap(t => t##items)
      ->Belt.Option.map(t =>
          {
            partial: "",
            commits:
              t->Belt.Array.keepMap(t => t)->Belt.Array.map(t => t##tag),
          }
        )
      ->Belt.Option.getWithDefault({partial: "", commits: [||]})
    );
  let (tagsFilterResults, setTagsFilterResults) = React.useState(() => None);

  let _ =
    React.useEffect2(
      () => {
        let highlightTags =
          highlight##tags
          ->Belt.Option.flatMap(t => t##items)
          ->Belt.Option.map(t => t->Belt.Array.keepMap(t => t))
          ->Belt.Option.getWithDefault([||]);

        let (createTagsInput, createHighlightTagsInput) = {
          let tagsToCreate =
            tagsState.commits
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
                Belt.Array.some(tagsState.commits, tag =>
                  highlightTag##tag##id === tag##id
                );
              !retained;
            })
          ->Belt.Array.map(highlightTag => {"id": highlightTag##id});

        let variables =
          UpdateHighlightMutation.makeVariables(
            ~updateHighlightInput={
              "id": highlight##id,
              "text": textState->Js.Option.some,
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

        let updateCache = () => {
          let cacheQuery =
            QueryRenderers_Notes_GraphQL.ListHighlights.Query.make(
              ~owner=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
              (),
            );
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
                        tagsState.commits
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
                                createHighlightTagsInput->Belt.Array.getBy(
                                  input =>
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
                        text: textState,
                        tags:
                          makeHighlightTagsConnection(
                            ~tags=updatedTags->Js.Option.some,
                          )
                          ->Js.Option.some,
                      };
                      let updatedHighlights =
                        Belt.Array.concatMany([|
                          Belt.Array.slice(
                            items,
                            ~offset=0,
                            ~len=itemIdx - 1,
                          ),
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

        let _ = handleSave(. variables, updateCache, updateHighlightMutation);

        None;
      },
      (textState, tagsState),
    );

  let _ =
    React.useEffect0(() => {
      Some(
        () => {
          let _ = Lodash.flush3(handleSave);
          ();
        },
      )
    });

  let handleTextChange = s => setTextState(_ => s);
  let handleTagsChange = (s: TextInput_Tags.Value.t) =>
    setTagsState(tagsState => {
      let updatedCommits =
        s.commits
        ->Belt.Array.map(text => {
            switch (
              Belt.Array.getBy(tagsState.commits, tag => tag##text === text),
              tagsFilterResults->Belt.Option.flatMap(r =>
                r->Belt.Array.getBy(tag => tag##text === text)
              ),
            ) {
            | (Some(tag), _) => tag
            | (_, Some(tag)) => tag
            | _ => {"id": Uuid.makeV4(), "text": text}
            }
          });

      {partial: s.partial, commits: updatedCommits};
    });
  let handleTagsFilterResults = s => setTagsFilterResults(_ => s);
  let handleTagsFilterClicked = tag =>
    setTagsState(tagsState => {
      {partial: "", commits: Belt.Array.concat(tagsState.commits, [|tag|])}
    });

  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "overflow-y-auto",
    ])}>
    <div className={cn(["px-6", "pb-4", "pt-16"])}>
      <TextInput.Highlight
        className={cn([styles##underline])}
        onTextChange=handleTextChange
        textValue=textState
        tagsValue={
          TextInput_Tags.Value.partial: tagsState.partial,
          commits: tagsState.commits->Belt.Array.map(t => t##text),
        }
        onTagsChange=handleTagsChange
      />
      {isActive && Js.String.length(tagsState.partial) > 0
         ? <QueryRenderers_TagsFilter
             text={tagsState.partial}
             onTagResults=handleTagsFilterResults
             onTagClicked=handleTagsFilterClicked
           />
         : React.null}
    </div>
  </div>;
};
