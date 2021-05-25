[@react.component]
let make = (~identityId, ~annotation, ~onAnnotationChange, ~onCollapse) => {
  let handleAnnotationChange = newAnnotation => {
    let _ = onAnnotationChange(newAnnotation);

    let allBodiesHaveId =
      newAnnotation.LiteralModel.Annotation.body
      ->Belt.Option.getWithDefault([||])
      ->Belt.Array.every(
          fun
          | TextualBody({id}) when Js.String2.startsWith(id, "https") => true
          | TextualBody(_) => false
          | _ => true,
        );

    let _ =
      if (allBodiesHaveId) {
        let _ =
          Webview.(
            postMessage(
              WebEvent.make(
                ~type_="EDIT_ANNOTATION_TAGS_RESULT",
                ~data=LiteralModel.Annotation.encode(newAnnotation),
                (),
              ),
            )
          );
        ();
      };
    ();
  };

  <div
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "relative",
      "overflow-y-auto",
    ])}>
    <Containers_NewAnnotationFromMessageEventHeader
      onDismiss={() => onCollapse()}
    />
    <Containers_NewAnnotationFromMessageEventEditor
      identityId
      annotation
      onAnnotationChange=handleAnnotationChange
    />
  </div>;
};
