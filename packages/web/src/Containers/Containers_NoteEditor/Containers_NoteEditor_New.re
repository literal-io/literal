open Styles;
open Containers_NoteEditor_New_GraphQL;

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

/** FIXME: restore cache update
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
**/
module PhaseTextInput = {
  [@react.component]
  let make = (~currentUser) => {
    let (editorValue, setEditorValue) =
      React.useState(() => Containers_NoteEditor_Base.{text: "", tags: [||]});
    let (createAnnotationMutation, _s, _f) =
      ApolloHooks.useMutation(CreateAnnotationMutation.definition);

    let handleSave = () => {
      let idPromise =
        Lib_GraphQL.Annotation.makeId(
          ~creatorUsername=
            AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
          ~textualTargetValue=editorValue.text,
        );

      let bodyPromise =
        editorValue.tags
        ->Belt.Array.map(tag =>
            Lib_GraphQL.AnnotationCollection.makeId(
              ~creatorUsername=
                AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
              ~label=tag##text,
            )
            |> Js.Promise.then_(id =>
                 Js.Promise.resolve({
                   "textualBody":
                     Some({
                       "id": Some(id),
                       "value": tag##text,
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
               )
          )
        ->Js.Promise.all;

      let _ =
        Js.Promise.all2((idPromise, bodyPromise))
        |> Js.Promise.then_(((id, body)) => {
             let input = {
               "context": [|Lib_GraphQL.Annotation.defaultContext|],
               "type": [|`ANNOTATION|],
               "id": id,
               "created": None,
               "modified": None,
               "generated": None,
               "audience": None,
               "canonical": None,
               "stylesheet": None,
               "via": None,
               "motivation": Some([|`HIGHLIGHTING|]),
               "creatorUsername":
                 AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
               "annotationGeneratorId": None,
               "body": None,
               "target": [|
                 {
                   "textualTarget":
                     Some({
                       "format": Some(`TEXT_PLAIN),
                       "language": Some(`EN_US),
                       "processingLanguage": Some(`EN_US),
                       "textDirection": Some(`LTR),
                       "accessibility": None,
                       "rights": None,
                       "value": editorValue.text,
                       "id": None,
                     }),
                   "externalTarget": None,
                 },
               |],
               "body": Js.Array2.length(body) > 0 ? Some(body) : None,
             };
             let variables =
               CreateAnnotationMutation.makeVariables(~input, ());

             let _ = createAnnotationMutation(~variables, ());
             /**
              let _ =
                updateCache(
                  ~currentUser,
                  ~editorValue,
                  ~createHighlightTagsInput,
                  ~createHighlightInput,
                );
              **/

             // FIXME: This should really do something like "back and replace"
             let _ = Next.Router.push("/notes?id=" ++ id);
             Js.Promise.resolve();
           });
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
          edge=MaterialUi.IconButton.Edge.start
          onClick={_ => onCreateFromText()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-75"]),
            },
          }
          classes={MaterialUi.IconButton.Classes.make(
            ~root=cn(["mr-20", "p-8"]),
            (),
          )}>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["w-16", "h-16", "pointer-events-none"])}
            icon=Svg.textFields
          />
        </MaterialUi.IconButton>
        <MaterialUi.IconButton
          size=`Medium
          edge=MaterialUi.IconButton.Edge.start
          onClick={_ => onCreateFromFile()}
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-75"]),
            },
          }
          classes={MaterialUi.IconButton.Classes.make(~root=cn(["p-8"]), ())}>
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
