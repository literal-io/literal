module AnnotationModel = QueryRenderers_NewAnnotationFromMessageEvent_AnnotationModel;

let textValueSelector = (~annotation: AnnotationModel.t) =>
  annotation.target
  ->Belt.Array.keepMap(target =>
      switch (target) {
      | AnnotationModel.Target.TextualTarget(target) => Some(target)
      | _ => None
      }
    )
  ->Belt.Array.get(0)
  ->Belt.Option.map(target => target.value)
  ->Belt.Option.getWithDefault("");

let tagsValueSelector = (~annotation: AnnotationModel.t, ~currentUser) =>
  annotation.body
  ->Belt.Option.map(bodies =>
      bodies->Belt.Array.keepMap(body =>
        switch (body) {
        | AnnotationModel.Body.TextualBody(body) =>
          let href =
            body.id
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
              text: body.value,
              id: body.id,
              href,
            },
          );
        | _ => None
        }
      )
    )
  ->Belt.Option.getWithDefault([||]);

[@react.component]
let make = (~annotation: AnnotationModel.t, ~currentUser, ~onAnnotationChange) => {
  let scrollContainerRef = React.useRef(Js.Nullable.null);
  let (pendingTagValue, setPendingTagValue) = React.useState(_ => "");

  let handleTagsChange = value => {
    let textualBodies =
      value->Belt.Array.map((tag: Containers_AnnotationEditor_Types.tag) =>
        AnnotationModel.Body.TextualBody({
          id: tag.id,
          value: tag.text,
          purpose: Some([|"TAGGING"|]),
          rights: None,
          accessibility: None,
          format: Some("TEXT_PLAIN"),
          textDirection: Some("LTR"),
          language: Some("EN_US"),
          processingLanguage: Some("EN_US"),
        })
      );
    let updatedBody =
      annotation.body
      ->Belt.Option.getWithDefault([||])
      ->Belt.Array.keep(body =>
          switch (body) {
          | AnnotationModel.Body.NotImplemented_Passthrough(_) => true
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
      Containers_AnnotationEditor_Types.{text: value, id: None, href: None};
    let _ = handleTagsChange(Belt.Array.concat(tagsValue, [|newTag|]));
    let _ =
      Lib_GraphQL.AnnotationCollection.makeId(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~label=value,
      )
      |> Js.Promise.then_(id => {
           let newTag =
             Containers_AnnotationEditor_Types.{
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
    <div className={Cn.fromList(["px-6", "py-16"])}>
      <TextInput.Annotation
        onTextChange={_ => ()}
        onTagsChange=handleTagsChange
        textValue={textValue}
        tagsValue
        placeholder="Lorem Ipsum"
        disabled=true
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
      <TextInput_Tags
        className={Cn.fromList(["px-2", "z-10", "bg-black"])}
        onValueChange=handlePendingTagChange
        onValueCommit=handlePendingTagCommit
        value=pendingTagValue
      />
    </div>
  </div>;
};
