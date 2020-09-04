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


let annotationFromCreateAnnotationInput = [%raw {|
  function (input) {
    return {
      ...input,
      __typename: "Annotation",
      created: (new Date()).toISOString(),
      body: 
        input.body
          ? input.body.map(body => { 
              const parser = [
                ["textualBody", { 
                  __typename: "TextualBody",
                  accessibility: null,
                  rights: null
                }],
                ["choiceBody", {__typename: "ChoiceBody" }],
                ["externalBody", {__typename: "ExternalBody"}],
                ["specificBody", {__typename: "SpecificBody"}]
              ]
              const [key, attrs] = parser.find(([key, _]) => body[key])
              return { ...attrs, ...body[key] }
          })
          : null,
      target:
        input.target
          ? input.target.map(target => {
              const parser = [
                ["textualTarget", { 
                  __typename: "TextualTarget",
                  textualTargetId: null,
                  rights: null,
                  accessibility: null
                }],
                ["externalTarget", {__typename: "ExternalTarget" }],
              ]
              const [key, attrs] = parser.find(([key, _]) => target[key])
              return { ...attrs, ...target[key] }

            })
          : null
    }
  }
|}];

let updateCache = (~currentUser, ~input) => {
  let cacheQuery =
    QueryRenderers_Notes_GraphQL.ListAnnotations.Query.make(
      ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      (),
    );
  let _ =
    QueryRenderers_Notes_GraphQL.ListAnnotations.readCache(
      ~query=cacheQuery,
      ~client=Provider.client,
      (),
    )
    ->Belt.Option.flatMap(cachedQuery => cachedQuery##listAnnotations)
    ->Belt.Option.flatMap(listAnnotations => listAnnotations##items)
    ->Belt.Option.forEach(annotations => {
        let newAnnotation = 
          input
          ->CreateAnnotationMutation.json_of_CreateAnnotationInput
          ->annotationFromCreateAnnotationInput;

        let newAnnotations =
          Belt.Array.concat(annotations, [|newAnnotation|]);

        let newData = {
          "listAnnotations":
            Some({
              "items": Some(newAnnotations),
              "__typename": "ModelAnnotationConnection",
            }),
          "__typename": "Query",
        };

        let _ =
          QueryRenderers_Notes_GraphQL.ListAnnotations.(
            writeCache(
              ~query=cacheQuery,
              ~client=Provider.client,
              ~data=newData,
              (),
            )
          );
        ();
      });
  ();
};

module PhaseTextInput = {
  [@react.component]
  let make = (~currentUser) => {
    let (editorValue, setEditorValue) =
      React.useState(() =>
        Containers_NoteEditor_Base_Types.{text: "", tags: [||]}
      );
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
          })
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
             let _ = updateCache(~currentUser, ~input);

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
