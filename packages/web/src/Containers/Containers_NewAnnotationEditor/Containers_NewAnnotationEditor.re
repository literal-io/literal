open Styles;
open Containers_NewAnnotationEditor_GraphQL;

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

  let _ =
    newAnnotation##body
    ->Belt.Option.map(bodies =>
        bodies->Belt.Array.keep(body =>
          body##purpose
          ->Js.Null.toOption
          ->Belt.Option.map(d => d->Belt.Array.some(p => p === "TAGGING"))
          ->Belt.Option.getWithDefault(false)
          &&
          body##__typename == "TextualBody"
          && body##id->Js.Null.toOption->Belt.Option.isSome
        )
      )
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.forEach(tag => {
        let cacheQuery =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=tag##id->Js.Null.toOption->Belt.Option.getExn,
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
        let newData =
          switch (data) {
          | Some(data) =>
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
                  [|
                    {
                      "__typename": "AnnotationPageItem",
                      "annotation": newAnnotation,
                    },
                  |],
                  items,
                ),
              );

            QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
              data,
              newItems,
            );
          | None => {
              "__typename": "Query",
              "getAnnotationCollection":
                Js.Null.return({
                  "__typename": "AnnotationCollection",
                  "label": tag##value,
                  "first":
                    Js.Null.return({
                      "__typename": "AnnotationPage",
                      "items":
                        Js.Null.return({
                          "__typename": "ModelAnnotationPageItemConnection",
                          "nextToken": Js.Null.return(""),
                          "items":
                            Js.Null.return([|
                              {
                                "__typename": "AnnotationPageItem",
                                "annotation": newAnnotation,
                              },
                            |]),
                        }),
                    }),
                }),
            }
          };
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
};

module PhaseTextInput = {
  [@react.component]
  let make = (~currentUser) => {
    let (textValue, setTextValue) = React.useState(() => "");
    let (tagsValue, setTagsValue) =
      React.useState(() =>
        [|
          TagsList.{
            text: Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel,
            id:
              Some(
                Lib_GraphQL.AnnotationCollection.(
                  makeIdFromComponent(
                    ~creatorUsername=
                      currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                    ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
                    (),
                  )
                ),
              ),
            href: None,
          },
        |]
      );

    let (createAnnotationMutation, _s, _f) =
      ApolloHooks.useMutation(CreateAnnotationMutation.definition);

    let handleSave = () => {
      let idPromise =
        Lib_GraphQL.Annotation.makeId(
          ~creatorUsername=
            AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
          ~textualTargetValue=textValue,
        );

      let bodyPromise =
        tagsValue
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
                       "value": textValue,
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
             let _ =
               Routes.CreatorsIdAnnotationCollectionsId.(
                 Next.Router.pushWithAs(
                   staticPath,
                   path(
                     ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                     ~creatorUsername=currentUser.username,
                   ),
                 )
               );
             Js.Promise.resolve();
           });
      ();
    };

    let handleTextChange = value => setTextValue(_ => value);

    <div
      className={Cn.fromList([
        "w-full",
        "h-full",
        "bg-black",
        "flex",
        "flex-col",
        "overflow-y-auto",
      ])}>
      <div className={Cn.fromList(["px-6", "pb-4", "pt-16"])}>
        <TextInput.Annotation
          onTextChange=handleTextChange
          textValue
          tagsValue
          autoFocus=true
        />
      </div>
      {Js.String.length(textValue) > 0
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
    </div>;
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
          onClick={_ => {
            let _ =
              Service_Analytics.(
                track(Click({action: "create from text", label: None}))
              );
            onCreateFromText();
          }}
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
          onClick={_ => {
            let _ =
              Service_Analytics.(
                track(Click({action: "create from file", label: None}))
              );
            onCreateFromFile();
          }}
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
