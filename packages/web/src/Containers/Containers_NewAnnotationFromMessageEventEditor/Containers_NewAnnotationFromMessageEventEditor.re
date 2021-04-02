let textValueSelector = (~annotation: Lib_WebView_Model_Annotation.t) =>
  annotation.target
  ->Belt.Array.keepMap(target =>
      switch (target) {
      | Lib_WebView_Model_Target.TextualTarget(target) => Some(target)
      | _ => None
      }
    )
  ->Belt.Array.get(0)
  ->Belt.Option.map(target => target.value)
  ->Belt.Option.getWithDefault("");

let tagsValueSelector =
    (~annotation: Lib_WebView_Model_Annotation.t, ~currentUser) =>
  annotation.body
  ->Belt.Option.map(bodies =>
      bodies->Belt.Array.keepMap(body =>
        switch (body) {
        | Lib_WebView_Model_Body.TextualBody(body) =>
          let href =
            Lib_GraphQL.AnnotationCollection.(
              makeIdFromComponent(
                ~annotationCollectionIdComponent=idComponent(body.id),
                ~creatorUsername=
                  currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                ~origin=Webapi.Dom.(window->Window.location->Location.origin),
                (),
              )
            );
          Some(
            Containers_AnnotationEditor_Tag.{
              text: body.value,
              id: Some(body.id),
              href: Some(href),
            },
          );
        | _ => None
        }
      )
    )
  ->Belt.Option.getWithDefault([||]);

[@react.component]
let make =
    (
      ~annotation: Lib_WebView_Model_Annotation.t,
      ~currentUser,
      ~onAnnotationChange,
    ) => {
  let scrollContainerRef = React.useRef(Js.Nullable.null);
  let (pendingTagValue, setPendingTagValue) = React.useState(_ => "");
  let previousAnnotation = React.useRef(annotation);
  let textInputRef = React.useRef(Js.Nullable.null);

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

  let handleTagsChange = value => {
    let textualBodies =
      value->Belt.Array.map((tag: Containers_AnnotationEditor_Tag.t) =>
        Lib_WebView_Model_Body.(
          TextualBody(
            makeTextualBody(
              ~id=tag.id->Belt.Option.getWithDefault(tag.text),
              ~value=tag.text,
              ~purpose=[|"TAGGING"|],
              ~format="TEXT_PLAIN",
              ~textDirection="LTR",
              ~language="EN_US",
              ~processingLanguage="EN_US",
              (),
            ),
          )
        )
      );
    let updatedBody =
      annotation.body
      ->Belt.Option.getWithDefault([||])
      ->Belt.Array.keep(body =>
          switch (body) {
          | Lib_WebView_Model_Body.NotImplemented_Passthrough(_) => true
          | _ => false
          }
        )
      ->Belt.Array.concat(textualBodies)
      ->Js.Option.some;

    let updatedAnnotation = {...annotation, body: updatedBody};
    let _ = onAnnotationChange(updatedAnnotation);
    ();
  };

  let textValue = textValueSelector(~annotation);
  let tagsValue = tagsValueSelector(~annotation, ~currentUser);

  let handlePendingTagChange = value => setPendingTagValue(_ => value);
  let handlePendingTagCommit = value => {
    let newTag =
      Containers_AnnotationEditor_Tag.{text: value, id: None, href: None};
    let _ = handleTagsChange(Belt.Array.concat(tagsValue, [|newTag|]));
    let _ = setPendingTagValue(_ => "");
    let _ =
      Lib_GraphQL.AnnotationCollection.makeId(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~label=value,
      )
      |> Js.Promise.then_(id => {
           let newTag =
             Containers_AnnotationEditor_Tag.{
               text: value,
               id: Some(id),
               href: None,
             };
           let _ =
             handleTagsChange(Belt.Array.concat(tagsValue, [|newTag|]));
           Js.Promise.resolve();
         });
    ();
  };

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
        onChange={_ => ()}
        value=textValue
        placeholder="Lorem Ipsum"
        disabled=true
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
      <TextInput_Tags
        className={Cn.fromList(["px-2", "z-10", "bg-black"])}
        onValueChange=handlePendingTagChange
        onValueCommit=handlePendingTagCommit
        value=pendingTagValue
        autoFocus=true
        placeholder="Add Tag..."
      />
    </div>
  </div>;
};
