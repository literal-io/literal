open Styles;
open Containers_NewAnnotationFromShareEditor_GraphQL;

[@react.component]
let make = (~annotationFragment as annotation, ~currentUser) => {
  let (textValue, setTextValue) =
    React.useState(() =>
      annotation##target
      ->Belt.Array.getBy(target =>
          switch (target) {
          | `TextualTarget(_) => true
          | `ExternalTarget(_) => false
          }
        )
      ->Belt.Option.flatMap(target =>
          switch (target) {
          | `TextualTarget(target) => Some(target##value)
          | `ExternalTarget(_) => None
          }
        )
      ->Belt.Option.getWithDefault("")
    );

  let (tagsValue, setTagsValue) =
    React.useState(() =>
      annotation##body
      ->Belt.Option.map(bodies =>
          bodies->Belt.Array.keepMap(body =>
            switch (body) {
            | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
              let href =
                body##id
                ->Belt.Option.map(id =>
                    Lib_GraphQL.AnnotationCollection.(
                      makeIdFromComponent(
                        ~annotationCollectionIdComponent=idComponent(id),
                        ~creatorUsername=
                          currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                        ~origin=
                          Webapi.Dom.(
                            window->Window.location->Location.origin
                          ),
                        (),
                      )
                    )
                  );
              Some(
                Containers_AnnotationEditor_Types.{
                  text: body##value,
                  id: body##id,
                  href,
                },
              );
            | `Nonexhaustive => None
            | _ => None
            }
          )
        )
      ->Belt.Option.getWithDefault([|
          {
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
        |])
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
                `TextualTarget(Js.Obj.assign(copy, {"value": textValue})),
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
              "value": textValue,
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
      tagsValue->Belt.Array.map(tag => {
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

  let handleTextChange = value => setTextValue(_ => value);
  let handleTagsChange = value => setTagsValue(_ => value);

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
        onTagsChange=handleTagsChange
        textValue
        tagsValue
        placeholder="Lorem Ipsum"
        autoFocus=true
      />
    </div>
    <FloatingActionButton
      onClick={_ev => handleSave()}
      className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["w-10", "h-10", "pointer-events-none"])}
        icon=Svg.done_
      />
    </FloatingActionButton>
  </div>;
};
