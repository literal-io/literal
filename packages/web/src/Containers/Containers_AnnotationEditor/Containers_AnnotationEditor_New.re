open Styles;
open Containers_AnnotationEditor_New_GraphQL;

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

let updateCache = (~currentUser, ~input) => {
  let newAnnotation =
    input
    ->CreateAnnotationMutation.json_of_CreateAnnotationInput
    ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput;

  let tags =
    newAnnotation##body
    ->Belt.Option.map(bodies =>
        bodies->Belt.Array.keepMap(body =>
          switch (body) {
          | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
            Some(body)
          | `Nonexhaustive => None
          | `TextualBody(_) => None
          }
        )
      )
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.map(tag => {
        let cacheQuery =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=tag##id->Belt.Option.getExn,
            (),
          );
        let data =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
            ~query=cacheQuery,
            ~client=Providers_Apollo.client,
            (),
          );
        let newAnnotation =
          input
          ->CreateAnnotationMutation.json_of_CreateAnnotationInput
          ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput;
        let cacheUpdate =
          switch (data) {
          | Some(data) =>
            let items =
              data##getAnnotationCollection
              ->Belt.Option.flatMap(d => d##first)
              ->Belt.Option.flatMap(d => d##items)
              ->Belt.Option.flatMap(d => d##items)
              ->Belt.Option.getWithDefault([||]);
            let newItems =
              Belt.Array.concat([|Some(newAnnotation)|], items);

            QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setCacheItems(
              data,
              newItems,
            );
          | None => {
              "getAnnotationCollection":
                Some({
                  "label": tag##text,
                  "first":
                    Some({
                      "items":
                        Some({
                          "items": Some({"annotation": Some(newAnnotation)}),
                        }),
                    }),
                }),
            }
          };
        ();
      });
  ();
};

/**
let updateCache = (~currentUser, ~input) => {
  let cacheQuery =
    QueryRenderers_Annotations_GraphQL.ListAnnotations.Query.make(
      ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      (),
    );
  let _ =
    QueryRenderers_Annotations_GraphQL.ListAnnotations.readCache(
      ~query=cacheQuery,
      ~client=Providers_Apollo.client,
      (),
    )
    ->Belt.Option.flatMap(cachedQuery => cachedQuery##listAnnotations)
    ->Belt.Option.flatMap(listAnnotations => listAnnotations##items)
    ->Belt.Option.forEach(annotations => {
        let newAnnotation =
          input
          ->CreateAnnotationMutation.json_of_CreateAnnotationInput
          ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput;

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
          QueryRenderers_Annotations_GraphQL.ListAnnotations.(
            writeCache(
              ~query=cacheQuery,
              ~client=Providers_Apollo.client,
              ~data=newData,
              (),
            )
          );
        ();
      });
  ();
};

**/
module PhaseTextInput = {
  [@react.component]
  let make = (~currentUser) => {
    let (editorValue, setEditorValue) =
      React.useState(() =>
        Containers_AnnotationEditor_Base_Types.{
          text: "",
          tags: [|
            {
              text: Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel,
              id: None,
            },
          |],
        }
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

             Js.log2("input", input);
             let variables =
               CreateAnnotationMutation.makeVariables(~input, ());

             let _ = createAnnotationMutation(~variables, ());
             let _ = updateCache(~currentUser, ~input);
             let _ =
               Routes.CreatorsIdAnnotationsId.(
                 Next.Router.pushWithAs(
                   staticPath,
                   path(
                     ~annotationIdComponent=
                       Lib_GraphQL.Annotation.idComponent(id),
                     ~creatorUsername=currentUser.username,
                   ),
                 )
               );
             Js.Promise.resolve();
           });
      ();
    };

    let handleChange = value => setEditorValue(_ => value);

    <>
      <Containers_AnnotationEditor_Base
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
