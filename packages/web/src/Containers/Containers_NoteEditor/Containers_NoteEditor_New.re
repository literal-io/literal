open Styles;
open Containers_NoteEditor_New_GraphQL;
open Containers_NoteEditor_GraphQL_Util;

[@bs.deriving jsConverter]
type phase = [ | `PhasePrompt | `PhaseTextInput];

let phase_encode = p => p->phaseToJs->Js.Json.string;
let phase_decode = json =>
  switch (json->Js.Json.decodeString->Belt.Option.flatMap(phaseFromJs)) {
  | Some(p) => Ok(p)
  | None =>
    Error(Decco.{path: "", message: "Not a phase value.", value: json})
  };

[@bs.deriving accessors]
type action =
  | SetPhase(phase);

let updateCache =
    (
      ~currentUser,
      ~editorValue: Containers_NoteEditor_Base.value,
      ~createHighlightInput,
      ~createHighlightTagsInput,
    ) => {
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
                editorValue.tags
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
                          makeTag(~id=input##tagId, ~text=committedTag##text),
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
                  ~text=editorValue.text,
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
  ();
};

module PhaseTextInput = {
  [@react.component]
  let make = (~currentUser) => {
    let (editorValue, setEditorValue) =
      React.useState(() => Containers_NoteEditor_Base.{text: "", tags: [||]});
    let (createHighlightMutation, _s, _f) =
      ApolloHooks.useMutation(CreateHighlightMutation.definition);

    let handleSave = () => {
      let highlightId = Uuid.makeV4();
      let createHighlightInput = {
        "id": highlightId,
        "text": editorValue.text,
        "createdAt": None,
        "note": None,
        "highlightScreenshotId": None,
        "owner": None,
      };
      let createTagsInput =
        editorValue.tags
        ->Belt.Array.keepMap(tag =>
            shouldCreateTag(tag)
              ? Some({"id": tag##id, "text": tag##text, "createdAt": None})
              : None
          );
      let createHighlightTagsInput =
        editorValue.tags
        ->Belt.Array.map(tag =>
            {
              "id":
                makeHighlightTagId(
                  ~highlightId=createHighlightInput##id,
                  ~tagId=tag##id,
                )
                ->Js.Option.some,
              "highlightId": createHighlightInput##id,
              "tagId": tag##id,
              "createdAt": None,
            }
          );

      let variables =
        CreateHighlightMutation.makeVariables(
          ~input={
            "createHighlight": createHighlightInput,
            "createTags":
              Js.Array2.length(createTagsInput) > 0
                ? Some(createTagsInput) : None,
            "createHighlightTags":
              Js.Array2.length(createHighlightTagsInput) > 0
                ? Some(createHighlightTagsInput) : None,
          },
          (),
        );
      let _ = createHighlightMutation(~variables, ());
      let _ =
        updateCache(
          ~currentUser,
          ~editorValue,
          ~createHighlightTagsInput,
          ~createHighlightInput,
        );
      // FIXME: This should really do something like "back and replace"
      let _ = Next.Router.push("/notes?id=" ++ highlightId);
      ();
    };

    let handleChange = value => setEditorValue(_ => value);

    <>
      <Containers_NoteEditor_Base
        onChange=handleChange
        autoFocus=true
        placeholder="Lorem Ipsum"
        currentUser
      />
      {Js.String.length(editorValue.text) > 0
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
          classes=[Root(cn(["mr-20", "p-8"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["w-16", "h-16", "pointer-events-none"])}
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
          classes=[Root(cn(["p-8"]))]>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["w-16", "h-16", "pointer-events-none"])}
            icon=Svg.textSnippet
          />
        </MaterialUi.IconButton>
      </div>
    </div>;
};

[@react.component]
let make = (~currentUser, ~initialPhaseState=`PhasePrompt) => {
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) => {
        switch (action) {
        | SetPhase(nextPhase) => nextPhase
        }
      },
      initialPhaseState,
    );

  let handleCreateFromText = () => {
    let _ = `PhaseTextInput->setPhase->dispatchPhaseAction;
    ();
  };
  let handleCreateFromFile = () =>
    /** TODO **/
    {
      ();
    };

  switch (phaseState) {
  | `PhasePrompt =>
    <PhasePrompt
      onCreateFromFile=handleCreateFromFile
      onCreateFromText=handleCreateFromText
    />
  | `PhaseTextInput => <PhaseTextInput currentUser />
  };
};
