let modifiedSelector = (~annotation) =>
  annotation##modified->Belt.Option.flatMap(Js.Json.decodeString);

let textualTargetSelector =
    (~annotation)
    : option({.. "externalTargetCard_TextualTargetFragment": 'a}) =>
  annotation##target
  ->Belt.Array.getBy(target =>
      switch (target) {
      | `TextualTarget(_) => true
      | _ => false
      }
    )
  ->Belt.Option.flatMap(target =>
      switch (target) {
      | `TextualTarget(target) => Some(target)
      | _ => None
      }
    );

let textValueSelector = (~annotation) =>
  textualTargetSelector(~annotation)
  ->Belt.Option.map(target => target##value)
  ->Belt.Option.getWithDefault("");

let tagsValueSelector = (~currentUser, ~annotation) =>
  annotation##body
  ->Belt.Option.map(bodies =>
      bodies->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
          let href =
            Lib_GraphQL.AnnotationCollection.(
              makeIdFromComponent(
                ~annotationCollectionIdComponent=idComponent(body##id),
                ~creatorUsername=
                  currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                ~origin=Webapi.Dom.(window->Window.location->Location.origin),
                (),
              )
            );
          Some(
            Containers_AnnotationEditor_Tag.{
              text: body##value,
              id: Some(body##id),
              href: Some(href),
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

let targetWithExternalTargetSelector = (~annotation) =>
  annotation##target
  ->Belt.Array.keepMap(target =>
      switch (target) {
      | `SpecificTarget(specficTarget) =>
        switch (specficTarget##source) {
        | `ExternalTarget(externalTarget) =>
          Some((specficTarget##specificTargetId, externalTarget))
        | _ => None
        }
      | _ => None
      }
    )
  ->Belt.Array.get(0);

let handleSave =
  Lodash.debounce2(
    (.
      variables,
      updateAnnotationMutation:
        ApolloHooks.Mutation.mutation(
          Containers_AnnotationEditor_GraphQL.PatchAnnotationMutation.t,
        ),
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
let make = (~annotationFragment as annotation, ~currentUser, ~isVisible) => {
  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_AnnotationEditor_GraphQL.PatchAnnotationMutation.definition,
    );
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

  let handleViewTargetForAnnotation =
      (~targetId, ~annotation, ~displayBottomSheet, ()) => {
    if (displayBottomSheet) {
      let _ =
        Service_Analytics.(
          track(Click({action: "open external target", label: None}))
        );
      ();
    };

    let _ =
      Webview.(
        postMessage(
          WebEvent.make(
            ~type_="VIEW_TARGET_FOR_ANNOTATION",
            ~data=
              Js.Json.object_(
                Js.Dict.fromList([
                  ("targetId", Js.Json.string(targetId)),
                  (
                    "annotation",
                    annotation
                    ->Containers_AnnotationEditor_GraphQL.Webview.makeAnnotation
                    ->Lib_WebView_Model_Annotation.encode,
                  ),
                  (
                    "displayBottomSheet",
                    Js.Json.boolean(displayBottomSheet),
                  ),
                ]),
              ),
            (),
          ),
        )
      );
    ();
  };

  let targetWithExternalTarget =
    targetWithExternalTargetSelector(~annotation);
  let targetId =
    switch (targetWithExternalTarget) {
    | Some((targetId, _)) => Some(targetId)
    | _ => None
    };
  let _ =
    React.useEffect2(
      () => {
        let _ =
          switch (targetId) {
          | Some(targetId) when isVisible =>
            handleViewTargetForAnnotation(
              ~targetId,
              ~annotation,
              ~displayBottomSheet=false,
              (),
            )
          | _ => ()
          };
        None;
      },
      (targetId, isVisible),
    );

  let handleTagsChange = newTagsValue => {
    let modifiedTs =
      Js.Date.(make()->toISOString)->Js.Json.string->Js.Option.some;
    let _ = setTagsValue(_ => ModifiedValue.make(newTagsValue));

    let textualBodies =
      tagsValue
      ->Belt.Array.map(tag =>
          tag
          |> Containers_AnnotationEditor_Tag.ensureId(
               ~creatorUsername=
                 AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
             )
          |> Js.Promise.then_(tag =>
               tag
               ->Containers_AnnotationEditor_Tag.asTextualBody
               ->Js.Promise.resolve
             )
        )
      ->Js.Promise.all;
    let newTextualBodies =
      newTagsValue
      ->Belt.Array.map(tag =>
          tag
          |> Containers_AnnotationEditor_Tag.ensureId(
               ~creatorUsername=
                 AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
             )
          |> Js.Promise.then_(tag =>
               tag
               ->Containers_AnnotationEditor_Tag.asTextualBody
               ->Js.Promise.resolve
             )
        )
      ->Js.Promise.all;

    let _ =
      Js.Promise.all2((textualBodies, newTextualBodies))
      |> Js.Promise.then_(((textualBodies, newTextualBodies)) => {
           let input =
             Lib_GraphQL_PatchAnnotationMutation.Input.(
               make(
                 ~id=annotation##id,
                 ~creatorUsername=
                   AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                 ~operations=
                   Lib_GraphQL_PatchAnnotationMutation.Input.makeFromBodyDiff(
                     ~oldBody=textualBodies->Belt.Array.keepMap(d => d),
                     ~newBody=newTextualBodies->Belt.Array.keepMap(d => d),
                   ),
               )
             );
           let _ =
             handleSave(.
               Containers_AnnotationEditor_GraphQL.PatchAnnotationMutation.makeVariables(
                 ~input,
                 (),
               ),
               patchAnnotationMutation,
             );
           let _ =
             Lib_GraphQL_PatchAnnotationMutation.Apollo.updateCache(
               ~currentUser,
               ~input,
             );
           Js.Promise.resolve();
         });

    ();
  };

  let handleTextChange = textValue => {
    let modifiedTs =
      Js.Date.(make()->toISOString)->Js.Json.string->Js.Option.some;
    let _ = setTextValue(_ => ModifiedValue.make(textValue));

    let updateTargetOperation = {
      let idx =
        annotation##target
        ->Belt.Array.getIndexBy(target =>
            switch (target) {
            | `TextualTarget(_) => true
            | _ => false
            }
          );
      idx
      ->Belt.Option.flatMap(idx => annotation##target->Belt.Array.get(idx))
      ->Belt.Option.map(target => {
          let operation =
            switch (target) {
            | `TextualTarget(target) =>
              let copy = Js.Obj.assign(Js.Obj.empty(), target);
              `TextualTarget(Js.Obj.assign(copy, {"value": textValue}))
              |> Lib_GraphQL_AnnotationTargetInput.makeFromTarget
              |> Js.Promise.then_(targetInput =>
                   targetInput
                   ->Belt.Option.map(targetInput =>
                       Lib_GraphQL_PatchAnnotationMutation.Input.(
                         makeOperation(
                           ~set=
                             makeSet(
                               ~where_=
                                 makeWhere(~id=target##textualTargetId, ()),
                               ~target=targetInput,
                               (),
                             ),
                           (),
                         )
                       )
                     )
                   ->Js.Promise.resolve
                 );
            | _ => Js.Promise.resolve(None)
            };

          let makeAddOperation = () =>
            Lib_GraphQL_PatchAnnotationMutation.Input.(
              makeOperation(
                ~add=
                  makeAdd(
                    ~target=
                      Lib_GraphQL_AnnotationTargetInput.make(
                        ~textualTarget=
                          Lib_GraphQL_AnnotationTargetInput.makeTextualTargetInput(
                            ~id=
                              Lib_GraphQL_AnnotationTargetInput.makeId(
                                ~annotationId=annotation##id,
                              ),
                            ~format=`TEXT_PLAIN,
                            ~language=`EN_US,
                            ~processingLanguage=`EN_US,
                            ~textDirection=`LTR,
                            ~value=textValue,
                            (),
                          ),
                        (),
                      ),
                    (),
                  ),
                (),
              )
            );

          operation
          |> Js.Promise.then_(operation =>
               operation
               ->Belt.Option.getWithDefault(makeAddOperation())
               ->Js.Promise.resolve
             );
        });
    };

    let _ =
      updateTargetOperation->Belt.Option.forEach(updateTargetOperation => {
        let _ =
          updateTargetOperation
          |> Js.Promise.then_(updateTargetOperation => {
               let input =
                 Lib_GraphQL_PatchAnnotationMutation.Input.make(
                   ~id=annotation##id,
                   ~creatorUsername=
                     AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                   ~operations=[|updateTargetOperation|],
                 );
               let _ =
                 handleSave(.
                   Containers_AnnotationEditor_GraphQL.PatchAnnotationMutation.makeVariables(
                     ~input,
                     (),
                   ),
                   patchAnnotationMutation,
                 );
               Js.Promise.resolve();
             });
        ();
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
    <div className={Cn.fromList(["px-4", "py-16"])}>
      {switch (targetWithExternalTarget, textualTargetSelector(~annotation)) {
       | (Some((targetId, externalTarget)), Some(textualTarget)) =>
         <ExternalTargetCard
           onClick={_ => {
             handleViewTargetForAnnotation(
               ~annotation,
               ~targetId,
               ~displayBottomSheet=true,
               (),
             )
           }}
           externalTargetFragment=
             externalTarget##externalTargetCard_ExternalTargetFragment
           textualTargetFragment=
             textualTarget##externalTargetCard_TextualTargetFragment
         />
       | _ =>
         <TextInput.Annotation
           onChange=handleTextChange
           value=textValue
           textInputRef
           inputClasses={MaterialUi.Input.Classes.make(
             ~root=Cn.fromList(["p-4", "bg-darkAccent", "rounded-sm"]),
             ~inputMultiline=Cn.fromList(["px-0"]),
             (),
           )}
         />
       }}
      <TagsList value=tagsValue onChange=handleTagsChange />
    </div>
  </div>;
};
