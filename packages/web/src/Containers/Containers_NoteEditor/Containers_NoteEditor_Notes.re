open Containers_NoteEditor_Notes_GraphQL;

let handleSave =
  Lodash.debounce2(
    (.
      variables,
      updateAnnotationMutation:
        ApolloHooks.Mutation.mutation(PatchAnnotationMutation.t),
    ) => {
      let _ = updateAnnotationMutation(~variables, ());
      ();
    },
    500,
  );

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
              "textualTargetId": None,
              "format": Some(`TEXT_PLAIN),
              "language": Some(`EN_US),
              "processingLanguage": Some(`EN_US),
              "textDirection": Some(`LTR),
              "accessibility": None,
              "rights": None,
              "value": editorValue.text,
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
