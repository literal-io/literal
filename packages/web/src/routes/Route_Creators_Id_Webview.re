type viewState =
  | EditAnnotationTags(Lib_WebView_Model.Annotation.t)
  | CollapsedAnnotationTags(Lib_WebView_Model.Annotation.t);

[@react.component]
let default = (~rehydrated) => {
  let (viewState, setViewState) = React.useState(() => None);

  let _ =
    React.useEffect0(() => {
      let handleEditAnnotationTags = (event: option(Js.Json.t)) => {
        event
        ->Belt.Option.map(Lib_WebView_Model.Annotation.decode)
        ->Belt.Option.forEach(result =>
            switch (result) {
            | Ok(annotation) =>
              setViewState(_ => Some(EditAnnotationTags(annotation)))
            | Error(e) => Js.log2("error", e)
            }
          );
      };

      let handleCollapsedAnnotationTags = (event: option(Js.Json.t)) => {
        event
        ->Belt.Option.map(Lib_WebView_Model.Annotation.decode)
        ->Belt.Option.forEach(result =>
            switch (result) {
            | Ok(annotation) =>
              setViewState(_ => Some(CollapsedAnnotationTags(annotation)))
            | Error(e) => Js.log2("error", e)
            }
          );
      };

      let events = [|
        ("VIEW_STATE_EDIT_ANNOTATION_TAGS", handleEditAnnotationTags),
        (
          "VIEW_STATE_COLLAPSED_ANNOTATION_TAGS",
          handleCollapsedAnnotationTags,
        ),
      |];

      let _ =
        events->Belt.Array.forEach(event =>
          Webview.WebEventHandler.register(event)
        );

      Some(
        () => {
          let _ =
            events->Belt.Array.forEach(((type_, _)) =>
              Webview.WebEventHandler.unregister(type_)
            );
          ();
        },
      );
    });

  let authentication = Hooks_CurrentUserInfo.use();

  let handleAnnotationChange = newAnnotation =>
    setViewState(viewState =>
      switch (viewState) {
      | Some(EditAnnotationTags(_)) =>
        Some(EditAnnotationTags(newAnnotation))
      | Some(CollapsedAnnotationTags(_)) =>
        Some(CollapsedAnnotationTags(newAnnotation))
      | None => None
      }
    );

  let handleSetState = viewState => {
    let _ = Service_Analytics.(track(Click({action: "close", label: None})));
    let _ =
      Webview.(
        postMessage(
          WebEvent.make(
            ~type_="SET_VIEW_STATE",
            ~data=
              Js.Json.object_(
                Js.Dict.fromList([("state", Js.Json.string(viewState))]),
              ),
            (),
          ),
        )
      );
    ();
  };

  switch (viewState) {
  | None => <Loading />
  | Some(EditAnnotationTags(annotation)) =>
    <QueryRenderers_WebView_EditAnnotationTags
      rehydrated
      authentication
      annotation
      onAnnotationChange=handleAnnotationChange
      onCollapse={() => handleSetState("COLLAPSED_ANNOTATION_TAGS")}
    />
  | Some(CollapsedAnnotationTags(annotation)) =>
    <QueryRenderers_WebView_CollapsedAnnotationTags
      rehydrated
      authentication
      annotation
      onExpand={() => handleSetState("EDIT_ANNOTATION_TAGS")}
    />
  };
};

let page = "creators/[creatorUsername]/webview.js";