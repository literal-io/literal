open Containers_NewAnnotationFromShareEditor_GraphQL;

[@react.component]
let make = (~annotationFragment as annotation, ~identityId) => {
  let scrollContainerRef = React.useRef(Js.Nullable.null);
  let textInputRef = React.useRef(Js.Nullable.null);
  let (textValue, setTextValue) =
    React.useState(() =>
      annotation##target
      ->Belt.Array.getBy(target =>
          switch (target) {
          | `TextualTarget(_) => true
          | _ => false
          }
        )
      ->Belt.Option.flatMap(target =>
          switch (target) {
          | `TextualTarget(target) => Some(target##value)
          | _ => None
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
                Lib_GraphQL.AnnotationCollection.(
                  makeIdFromComponent(
                    ~annotationCollectionIdComponent=idComponent(body##id),
                    ~identityId,
                    ~origin=
                      Webapi.Dom.(window->Window.location->Location.origin),
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
          {
            text: Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel,
            id:
              Some(
                Lib_GraphQL.AnnotationCollection.(
                  makeIdFromComponent(
                    ~identityId,
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
    let updateTargetOperation =
      annotation##target
      ->Belt.Array.getIndexBy(target =>
          switch (target) {
          | `TextualTarget(_) => true
          | _ => false
          }
        )
      ->Belt.Option.flatMap(idx => annotation##target->Belt.Array.get(idx))
      ->Belt.Option.flatMap(target =>
          switch (target) {
          | `TextualTarget(target) =>
            Lib_GraphQL_PatchAnnotationMutation.Input.(
              makeOperation(
                ~set=
                  makeSet(
                    ~where_=makeWhere(~id=target##textualTargetId, ()),
                    ~target=
                      Lib_GraphQL_AnnotationTargetInput.make(
                        ~textualTarget=
                          Lib_GraphQL_AnnotationTargetInput.makeTextualTargetInput(
                            ~value=textValue,
                            ~id=target##textualTargetId,
                            ~format=?target##format,
                            ~processingLanguage=?target##processingLanguage,
                            ~accessibility=?target##accessibility,
                            ~rights=?target##rights,
                            (),
                          ),
                        (),
                      ),
                    (),
                  ),
                (),
              )
            )
            ->Js.Option.some
          | _ => None
          }
        )
      ->Belt.Option.getWithDefault(
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
          ),
        );

    let _ =
      tagsValue
      ->Belt.Array.map(tag =>
          tag
          |> Containers_AnnotationEditor_Tag.ensureId(~identityId)
          |> Js.Promise.then_(tag =>
               tag
               ->Containers_AnnotationEditor_Tag.asTextualBody
               ->Js.Promise.resolve
             )
        )
      ->Js.Promise.all
      |> Js.Promise.then_(newBody => {
           let input =
             Lib_GraphQL_PatchAnnotationMutation.Input.make(
               ~id=annotation##id,
               ~creatorUsername=identityId,
               ~operations=
                 Belt.Array.concat(
                   Lib_GraphQL_PatchAnnotationMutation.Input.makeFromBodyDiff(
                     ~oldBody=
                       annotation##body->Belt.Option.getWithDefault([||]),
                     ~newBody=newBody->Belt.Array.keepMap(d => d),
                   ),
                   [|updateTargetOperation|],
                 ),
             );
           let variables = PatchAnnotationMutation.makeVariables(~input, ());
           let mutationResult = patchAnnotationMutation(~variables, ());
           let _ =
             Lib_GraphQL_PatchAnnotationMutation.Apollo.updateCache(
               ~identityId,
               ~input,
             );

           let _ =
             if (!Webview.isWebview()) {
               Routes.CreatorsIdAnnotationCollectionsId.(
                 Next.Router.pushWithAs(
                   staticPath,
                   path(
                     ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                     ~identityId,
                   ),
                 )
               );
             };

           mutationResult;
         })
      |> Js.Promise.then_(_ => {
           let _ =
             Webview.(
               postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ()))
             );
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
            Containers_AnnotationEditor_Tag.{
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
      Lib_GraphQL.AnnotationCollection.makeId(~identityId, ~label=value)
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
                         Containers_AnnotationEditor_Tag.{
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
              Raw.maybeScrollTo(
                scrollContainerElem,
                {"top": targetTop, "left": 0., "behavior": "smooth"},
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
    <div className={Cn.fromList(["px-4", "py-16"])}>
      <TextInput.Annotation
        onChange=handleTextChange
        value=textValue
        placeholder="Lorem Ipsum"
        autoFocus=true
        textInputRef
        inputClasses={MaterialUi.Input.Classes.make(
          ~root=Cn.fromList(["p-4", "bg-darkAccent", "rounded-sm"]),
          ~inputMultiline=Cn.fromList(["px-0"]),
          (),
        )}
      />
      <TagsList value=tagsValue onChange=handleTagsChange />
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
          className={Cn.fromList(["w-10", "h-10", "pointer-events-none"])}
          icon=Svg.done_
        />
      </FloatingActionButton>
    </div>
  </div>;
};
