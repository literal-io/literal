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
  [@react.component]
  let make = (~currentUser) => {
    let (createHighlightMutation, _s, _f) =
      ApolloHooks.useMutation(CreateHighlightMutation.definition);

    let (textState, setTextState) = React.useState(() => "");
    let (tagsState, setTagsState) =
      React.useState(() => TextInput.Tags.Value.empty());

    let handleSave = () => {
      let highlightId = Uuid.makeV4();
      let variables =
        CreateHighlightMutation.makeVariables(
          ~input={
            "id": highlightId,
            "text": textState,
            "createdAt": None,
            "note": None,
            "highlightScreenshotId": None,
            "owner": None,
          },
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
              ->Belt.Option.flatMap(items)
              ->Belt.Option.map(items => {
                  let updatedItems =
                    Js.Array.concat(
                      [|
                        Some({
                          id: highlightId,
                          createdAt: Js.Date.(make()->toISOString),
                          text: textState,
                          typename: highlightTypename,
                        }),
                      |],
                      items,
                    );
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
    let handleTagsChange = s => setTagsState(_ => s);

    <>
      <div className={cn(["px-6", "pt-4", "pb-24"])}>
        <TextInput_Highlight
          onTextChange=handleTextChange
          textValue=textState
          onTagsChange=handleTagsChange
          tagsValue=tagsState
          placeholder="Lorem ipsum"
          autoFocus=true
        />
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
