open Styles;
open Containers_NewNoteEditor_GraphQL;

[@bs.deriving accessors]
type phase =
  | PhasePrompt
  | PhaseTextInput;

[@bs.deriving accessors]
type action =
  | SetPhase(phase);

module PhaseTextInput = {
  type tagState = {
    commits:
      array({
        .
        "id": string,
        "text": string,
      }),
    partial: string,
    filterResults:
      option(
        array({
          .
          "text": string,
          "id": string,
        }),
      ),
  };

  [@react.component]
  let make = (~currentUser) => {
    let (createHighlightMutation, _s, _f) =
      ApolloHooks.useMutation(CreateHighlightMutation.definition);

    let (textState, setTextState) = React.useState(() => "");
    let (tagsState, setTagsState) =
      React.useState(() => {partial: "", commits: [||], filterResults: None});

    let handleSave = () => {
      let createHighlightInput = {
        "id": Uuid.makeV4(),
        "text": textState,
        "createdAt": None,
        "note": None,
        "highlightScreenshotId": None,
        "owner": None,
      };
      let createTagsInput =
        tagsState.commits
        ->Belt.Array.map(tag =>
            {"id": tag##id, "text": tag##text, "createdAt": None}
          );
      let createHighlightTagsInput =
        createTagsInput->Belt.Array.map(tag =>
          {
            "id": Some(Uuid.makeV4()),
            "highlightId": createHighlightInput##id,
            "tagId": tag##id,
            "createdAt": None,
          }
        );

      let variables =
        CreateHighlightMutation.makeVariables(
          ~createHighlightInput,
          ~createTagsInput,
          ~createHighlightTagsInput,
          (),
        );
      let _ = createHighlightMutation(~variables, ());
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
              ->Belt.Option.map(items => {
                  let updatedTags =
                    tagsState.commits
                    ->Belt.Array.map(committedTag =>
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
                      );

                  let updatedHighlight =
                    makeHighlight(
                      ~id=createHighlightInput##id,
                      ~createdAt=Js.Date.(make()->toISOString),
                      ~text=textState,
                      ~tags=
                        makeHighlightTagsConnection(
                          ~tags=updatedTags->Js.Option.some,
                        )
                        ->Js.Option.some,
                    )
                    ->Js.Option.some;
                  let updatedItems =
                    Js.Array.concat([|updatedHighlight|], items);
                  {
                    ...cachedQuery,
                    listHighlights:
                      Some({
                        ...cachedQuery->listHighlights->Belt.Option.getExn,
                        items: Some(updatedItems),
                      }),
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

      let _ = Next.Router.back();
      ();
    };

    let handleTextChange = s => setTextState(_ => s);
    let handleTagsChange = (s: TextInput_Tags.Value.t) =>
      setTagsState(tagsState => {
        let updatedCommits =
          s.commits
          ->Belt.Array.map(text => {
              switch (
                Belt.Array.getBy(tagsState.commits, tag => tag##text === text),
                tagsState.filterResults
                ->Belt.Option.flatMap(r =>
                    r->Belt.Array.getBy(tag => tag##text === text)
                  ),
              ) {
              | (Some(tag), _) => tag
              | (_, Some(tag)) => tag
              | _ => {"id": Uuid.makeV4(), "text": text}
              }
            });
        {
          partial: s.partial,
          commits: updatedCommits,
          filterResults: tagsState.filterResults,
        };
      });

    let handleTagsFilterResults = s =>
      setTagsState(tagsState => {...tagsState, filterResults: s});

    let handleTagsFilterClicked = tag =>
      setTagsState(tagsState =>
        {
          partial: "",
          filterResults: tagsState.filterResults,
          commits: Belt.Array.concat(tagsState.commits, [|tag|]),
        }
      );

    <>
      <div className={cn(["px-6", "pt-4", "pb-24"])}>
        <TextInput_Highlight
          onTextChange=handleTextChange
          textValue=textState
          onTagsChange=handleTagsChange
          tagsValue={
            TextInput_Tags.Value.partial: tagsState.partial,
            commits: tagsState.commits->Belt.Array.map(t => t##text),
          }
          placeholder="Lorem ipsum"
          autoFocus=true
        />
        {Js.String.length(tagsState.partial) > 0
           ? <QueryRenderers_TagsFilter
               text={tagsState.partial}
               onTagResults=handleTagsFilterResults
               onTagClicked=handleTagsFilterClicked
             />
           : React.null}
      </div>
      {Js.String.length(textState) > 0
         ? <FloatingActionButton
             onClick={_ev => handleSave()}
             className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}>
             <Svg
               placeholderViewBox="0 0 24 24"
               className={cn(["w-10", "h-10", "pointer-events-none"])}
               icon=Svg.done_
             />
           </FloatingActionButton>
         : React.null}
    </>;
  };
};

module PhasePrompt = {
  [@react.component]
  let make = (~onCreateFromText, ~onCreateFromFile) =>
    <div
      className={cn([
        "flex",
        "flex-row",
        "justify-center",
        "items-start",
        "flex-auto",
        "pt-20",
      ])}>
      <div
        className={cn([
          "flex",
          "flex-row",
          "flex-auto",
          "justify-center",
          "items-center",
          "pb-4",
          "mx-6",
          "border-b",
          "border-white-o50",
        ])}>
        <MaterialUi.IconButton
          size=`Medium
          edge=`Start
          onClick={_ => onCreateFromText()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-75"]),
            },
          }
          classes=[Root(cn(["p-0", "mr-20"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["w-16", "h-16", "m-8", "pointer-events-none"])}
            icon=Svg.textFields
          />
        </MaterialUi.IconButton>
        <MaterialUi.IconButton
          size=`Medium
          edge=`Start
          onClick={_ => onCreateFromFile()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-75"]),
            },
          }
          classes=[Root(cn(["p-0"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["w-16", "h-16", "m-8", "pointer-events-none"])}
            icon=Svg.textSnippet
          />
        </MaterialUi.IconButton>
      </div>
    </div>;
};

[@react.component]
let make = (~currentUser) => {
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) => {
        switch (action) {
        | SetPhase(nextPhase) => nextPhase
        }
      },
      PhasePrompt,
    );

  let handleCreateFromText = () => {
    let _ = phaseTextInput->setPhase->dispatchPhaseAction;
    ();
  };
  let handleCreateFromFile = () =>
    /** TODO **/
    {
      ();
    };

  switch (phaseState) {
  | PhasePrompt =>
    <PhasePrompt
      onCreateFromFile=handleCreateFromFile
      onCreateFromText=handleCreateFromText
    />
  | PhaseTextInput => <PhaseTextInput currentUser />
  };
};
