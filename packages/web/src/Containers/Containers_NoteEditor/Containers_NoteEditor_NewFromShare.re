open Styles;
open Containers_NoteEditor_NewFromShare_GraphQL;

[@react.component]
let make = (~annotationFragment as annotation, ~currentUser) => {
  let (editorValue, setEditorValue) =
    React.useState(() =>
      Containers_NoteEditor_Base_Types.{text: "", tags: [||]}
    );

  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(PatchAnnotationMutation.definition);

  let handleSave = () => {
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
              "textualTargetId": None,
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
           patchAnnotationMutation(~variables, ());
         })
      |> Js.Promise.then_(_ => {
           let _ =
             Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH")));
           Js.Promise.resolve();
         });
    ();
  };

  let handleChange = value => setEditorValue(_ => value);

  <>
    <Containers_NoteEditor_Base
      annotationFragment=annotation
      onChange=handleChange
      autoFocus=true
      placeholder="Lorem Ipsum"
      isActive=true
      currentUser
    />
    <FloatingActionButton
      onClick={_ev => handleSave()}
      className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["w-10", "h-10", "pointer-events-none"])}
        icon=Svg.done_
      />
    </FloatingActionButton>
  </>;
};
