open Containers_AnnotationEditor_GraphQL;

let modifiedSelector = (~annotation) =>
  annotation##modified->Belt.Option.flatMap(Js.Json.decodeString);

let textValueSelector = (~annotation) =>
  annotation##target
  ->Belt.Array.getBy(target =>
      switch (target) {
      | `TextualTarget(_) => true
      | `ExternalTarget(_) => false
      | `SpecificTarget(_) => false
      }
    )
  ->Belt.Option.flatMap(target =>
      switch (target) {
      | `TextualTarget(target) => Some(target##value)
      | `ExternalTarget(_) => None
      | `SpecificTarget(_) => None
      }
    )
  ->Belt.Option.getWithDefault("");

let tagsValueSelector = (~currentUser, ~annotation) =>
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
                      Webapi.Dom.(window->Window.location->Location.origin),
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
      Lib_GraphQL.AnnotationCollection.{
        text: recentAnnotationCollectionLabel,
        id:
          makeIdFromComponent(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
            (),
          )
          ->Js.Option.some,
        href: None,
      },
    |]);

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

module ModifiedValue = {
  type t('a) = {
    modified: option(string),
    value: 'a,
  };
  let mostRecent = (a, b) =>
    switch (a.modified, b.modified) {
    | (Some(aModified), Some(bModified)) =>
      Js.Date.(aModified->fromString->valueOf)
      > Js.Date.(bModified->fromString->valueOf)
        ? a : b
    | (Some(_), None) => a
    | (None, Some(_)) => b
    | (None, None) => a
    };

  let make = value => {
    value,
    modified: Js.Date.(make()->toISOString)->Js.Option.some,
  };
};

[@react.component]
let make = (~annotationFragment as annotation, ~currentUser) => {
  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(PatchAnnotationMutation.definition);
  let scrollContainerRef = React.useRef(Js.Nullable.null);
  let textInputRef = React.useRef(Js.Nullable.null);
  let previousAnnotation = React.useRef(annotation);

  let (ModifiedValue.{value: textValue}, setTextValue) =
    React.useState(() =>
      ModifiedValue.{
        value: textValueSelector(~annotation),
        modified: modifiedSelector(~annotation),
      }
    );

  let _ =
    React.useEffect1(
      () => {
        let _ =
          setTextValue(currentModified => {
            let newModified =
              ModifiedValue.{
                value: textValueSelector(~annotation),
                modified: modifiedSelector(~annotation),
              };

            ModifiedValue.mostRecent(currentModified, newModified);
          });
        None;
      },
      [|annotation|],
    );

  let (ModifiedValue.{value: tagsValue}, setTagsValue) =
    React.useState(() =>
      ModifiedValue.{
        value: tagsValueSelector(~currentUser, ~annotation),
        modified: modifiedSelector(~annotation),
      }
    );

  let _ =
    React.useEffect2(
      () => {
        let _ =
          setTagsValue(currentModified => {
            let newModified =
              ModifiedValue.{
                value: tagsValueSelector(~currentUser, ~annotation),
                modified: modifiedSelector(~annotation),
              };

            ModifiedValue.mostRecent(currentModified, newModified);
          });
        None;
      },
      (annotation, currentUser),
    );

  let handleTagsChange = tagsValue => {
    let modifiedTs =
      Js.Date.(make()->toISOString)->Js.Json.string->Js.Option.some;
    let _ = setTagsValue(_ => ModifiedValue.make(tagsValue));
    let tagsWithIds =
      tagsValue
      ->Belt.Array.map((tag: Containers_AnnotationEditor_Types.tag) => {
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
               Js.Promise.resolve({...tag, id: Some(id)})
             );
        })
      ->Js.Promise.all;

    let _ =
      tagsWithIds
      |> Js.Promise.then_(tags => {
           let updateBody =
             tags->Belt.Array.map(
               (tag: Containers_AnnotationEditor_Types.tag) =>
               {
                 "textualBody":
                   Some({
                     "id": tag.id,
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
               }
             );
           let input = {
             "id": annotation##id,
             "creatorUsername":
               AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
             "operations": [|
               {
                 "set":
                   Some({
                     "body": Some(updateBody),
                     "target": None,
                     "modified": None,
                   }),
               },
               {
                 "set":
                   Some({
                     "body": None,
                     "target": None,
                     "modified": modifiedTs,
                   }),
               },
             |],
           };
           let variables = PatchAnnotationMutation.makeVariables(~input, ());
           let _ = handleSave(. variables, patchAnnotationMutation);
           let _ =
             Containers_AnnotationEditor_Apollo.updateCache(
               ~annotation,
               ~tags,
               ~currentUser,
               ~patchAnnotationMutationInput=input,
             );
           Js.Promise.resolve();
         });
    ();
  };

  let handleTextChange = textValue => {
    let modifiedTs =
      Js.Date.(make()->toISOString)->Js.Json.string->Js.Option.some;
    let _ = setTextValue(_ => ModifiedValue.make(textValue));

    let updateTargetInput = {
      let idx =
        annotation##target
        ->Belt.Array.getIndexBy(target =>
            switch (target) {
            | `TextualTarget(_) => true
            | `ExternalTarget(_)
            | `SpecificTarget(_) => false
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
            | `ExternalTarget(_)
            | `SpecificTarget(_) => None
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

      updatedTarget
      ->Belt.Array.map(Lib_GraphQL.Annotation.targetInputFromTarget)
      ->Js.Promise.all;
    };

    let _ =
      updateTargetInput
      |> Js.Promise.then_(updateTargetInput => {
           let variables =
             PatchAnnotationMutation.makeVariables(
               ~input={
                 "id": annotation##id,
                 "creatorUsername":
                   AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                 "operations": [|
                   {
                     "set":
                       Some({
                         "body": None,
                         "target": Some(updateTargetInput),
                         "modified": None,
                       }),
                   },
                   {
                     "set":
                       Some({
                         "body": None,
                         "target": None,
                         "modified": modifiedTs,
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

  let _ =
    React.useEffect1(
      () => {
        let currentTags = tagsValueSelector(~annotation, ~currentUser);
        let previousTags =
          tagsValueSelector(
            ~annotation=previousAnnotation.current,
            ~currentUser,
          );

        // scroll the newly added tag into view
        let _ =
          switch (
            Js.Nullable.toOption(scrollContainerRef.current),
            Js.Nullable.toOption(textInputRef.current),
          ) {
          | (Some(scrollContainerElem), Some(textInputElem))
              when
                Js.Array2.length(currentTags)
                > Js.Array2.length(previousTags) =>
            let rect =
              Webapi.Dom.Element.getBoundingClientRect(textInputElem);
            let targetTop =
              Webapi.Dom.DomRect.top(rect)
              +. Webapi.Dom.DomRect.height(rect)
              +. Webapi.Dom.(Window.scrollY(window));

            let _ =
              Webapi.Dom.Element.scrollToWithOptions(
                {"top": targetTop, "left": 0., "behavior": "smooth"},
                scrollContainerElem,
              );
            ();
          | _ => ()
          };

        previousAnnotation.current = annotation;
        None;
      },
      [|annotation|],
    );

  <div
    ref={scrollContainerRef->ReactDOMRe.Ref.domRef}
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "overflow-y-auto",
    ])}>
    <div className={Cn.fromList(["px-6", "py-16"])}>
      <TextInput.Annotation
        onTextChange=handleTextChange
        onTagsChange=handleTagsChange
        textValue
        tagsValue
        textInputRef
      />
    </div>
  </div>;
};
