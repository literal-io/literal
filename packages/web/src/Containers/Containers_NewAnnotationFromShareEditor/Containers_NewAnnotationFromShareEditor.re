open Containers_NewAnnotationFromShareEditor_GraphQL;

[@react.component]
let make = (~annotationFragment as annotation, ~currentUser) => {
  let scrollContainerRef = React.useRef(Js.Nullable.null);
  let textInputRef = React.useRef(Js.Nullable.null);
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
  let previousTagsValue = React.useRef(tagsValue);
  let (pendingTagValue, setPendingTagValue) = React.useState(_ => "");

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
    let tagsWithIds =
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
               Js.Promise.resolve({...tag, id: Some(id)})
             );
        })
      ->Js.Promise.all;

    let updateBodyInput =
      tagsWithIds
      |> Js.Promise.then_(tags =>
           tags
           ->Belt.Array.map((tag: Containers_AnnotationEditor_Types.tag) =>
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
             )
           ->Js.Promise.resolve
         );

    let _ =
      updateBodyInput
      |> Js.Promise.then_(updateBodyInput => {
           let input = {
             "id": annotation##id,
             "creatorUsername":
               AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
             "operations": [|
               {
                 "set":
                   Some({
                     "body": Some(updateBodyInput),
                     "target": None,
                     "modified": None,
                   }),
               },
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
                     "modified":
                       Js.Date.(make()->toISOString)
                       ->Js.Json.string
                       ->Js.Option.some,
                   }),
               },
             |],
           };
           let _ =
             tagsWithIds
             |> Js.Promise.then_(tags => {
                  let _ =
                    Containers_NewAnnotationFromShareEditor_Apollo.updateCache(
                      ~annotation,
                      ~tags,
                      ~currentUser,
                      ~patchAnnotationMutationInput=input,
                    );
                  Js.Promise.resolve();
                });
           let variables = PatchAnnotationMutation.makeVariables(~input, ());
           let mutationResult = patchAnnotationMutation(~variables, ());

           let _ =
             if (!Webview.isWebview()) {
               Routes.CreatorsIdAnnotationCollectionsId.(
                 Next.Router.pushWithAs(
                   staticPath,
                   path(
                     ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                     ~creatorUsername=currentUser.username,
                   ),
                 )
               );
             };

           mutationResult;
         })
      |> Js.Promise.then_(_ => {
           let _ =
             Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ())));
           Js.Promise.resolve();
         });
    ();
  };

  let handleTextChange = value => setTextValue(_ => value);
  let handleTagsChange = value => setTagsValue(_ => value);

  let handlePendingTagChange = value => setPendingTagValue(_ => value);
  let handlePendingTagCommit = value => {
    let _ =
      setTagsValue(tags =>
        Js.Array2.concat(
          [|
            Containers_AnnotationEditor_Types.{
              text: value,
              id: None,
              href: None,
            },
          |],
          tags,
        )
      );
    let _ = setPendingTagValue(_ => "");
    let _ =
      Lib_GraphQL.AnnotationCollection.makeId(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~label=value,
      )
      |> Js.Promise.then_(id => {
           let _ =
             setTagsValue(tags => {
               tags
               ->Belt.Array.getIndexBy(tag => tag.text === value)
               ->Belt.Option.map(idx => {
                   let copy = Js.Array2.copy(tags);
                   let _ =
                     Js.Array2.spliceInPlace(
                       tags,
                       ~pos=idx,
                       ~remove=1,
                       ~add=[|
                         Containers_AnnotationEditor_Types.{
                           text: value,
                           id: Some(id),
                           href: None,
                         },
                       |],
                     );
                   copy;
                 })
               ->Belt.Option.getWithDefault(tags)
             });
           Js.Promise.resolve();
         });
    ();
  };

  let _ =
    React.useEffect1(
      () => {
        // scroll the newly added tag into view
        let _ =
          switch (
            Js.Nullable.toOption(scrollContainerRef.current),
            Js.Nullable.toOption(textInputRef.current),
          ) {
          | (Some(scrollContainerElem), Some(textInputElem))
              when
                Js.Array2.length(tagsValue)
                > Js.Array2.length(previousTagsValue.current) =>
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

        previousTagsValue.current = tagsValue;
        None;
      },
      [|tagsValue|],
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
        placeholder="Lorem Ipsum"
        autoFocus=true
        textInputRef
      />
    </div>
    <div
      className={Cn.fromList([
        "absolute",
        "bottom-0",
        "left-0",
        "right-0",
        "flex",
        "flex-col",
        "items-end",
      ])}>
      <FloatingActionButton
        onClick={_ev => handleSave()}
        className={Cn.fromList(["m-6", "z-10"])}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={Cn.fromList(["w-10", "h-10", "pointer-events-none"])}
          icon=Svg.done_
        />
      </FloatingActionButton>
      <TextInput_Tags
        className={Cn.fromList(["px-2", "z-10", "bg-black"])}
        onValueChange=handlePendingTagChange
        onValueCommit=handlePendingTagCommit
        value=pendingTagValue
      />
    </div>
  </div>;
};
