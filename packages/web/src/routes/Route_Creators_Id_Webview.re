type viewState =
  | EditAnnotationTags(LiteralModel.Annotation.t)
  | CollapsedAnnotationTags(LiteralModel.Annotation.t);

[@react.component]
let default = (~rehydrated) => {
  let (viewState, setViewState) = React.useState(() => None);

  let _ =
    React.useEffect0(() => {
      let handleEditAnnotationTags = (event: option(Js.Json.t)) => {
        event
        ->Belt.Option.map(LiteralModel.Annotation.decode)
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
        ->Belt.Option.map(LiteralModel.Annotation.decode)
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

  let Providers_Authentication.{user} =
    React.useContext(Providers_Authentication.authenticationContext);

  let handleAnnotationChange = newAnnotation => {
    setViewState(viewState =>
      switch (viewState) {
      | Some(EditAnnotationTags(_)) =>
        Some(EditAnnotationTags(newAnnotation))
      | Some(CollapsedAnnotationTags(_)) =>
        Some(CollapsedAnnotationTags(newAnnotation))
      | None => None
      }
    );
    ();
  };

  let handleSetState = newViewState => {
    let _ = Service_Analytics.(track(Click({action: "close", label: None})));
    let _ =
      switch (viewState, newViewState) {
      | (Some(EditAnnotationTags(annotation)), "COLLAPSED_ANNOTATION_TAGS") =>
        let _ =
          Webview.(
            postMessage(
              WebEvent.make(
                ~type_="EDIT_ANNOTATION_TAGS_RESULT",
                ~data=annotation->LiteralModel.Annotation.encode,
                (),
              ),
            )
          );
        ();
      | _ => ()
      };

    let _ =
      Webview.(
        postMessage(
          WebEvent.make(
            ~type_="SET_VIEW_STATE",
            ~data=
              Js.Json.object_(
                Js.Dict.fromList([("state", Js.Json.string(newViewState))]),
              ),
            (),
          ),
        )
      );
    ();
  };

  switch (user, viewState) {
  | (_, None)
  | (SignedOutPromptAuthentication, _)
  | (Unknown, _) => <Loading />
  | _ when !rehydrated => <Loading />
  | (GuestUser({identityId}), Some(EditAnnotationTags(annotation)))
  | (SignedInUser({identityId}), Some(EditAnnotationTags(annotation)))
  | (
      SignedInUserMergingIdentites({identityId}),
      Some(EditAnnotationTags(annotation)),
    ) =>
    <QueryRenderers_WebView_EditAnnotationTags
      identityId
      annotation
      onAnnotationChange=handleAnnotationChange
      onCollapse={() => handleSetState("COLLAPSED_ANNOTATION_TAGS")}
    />
  | (GuestUser({identityId}), Some(CollapsedAnnotationTags(annotation)))
  | (SignedInUser({identityId}), Some(CollapsedAnnotationTags(annotation)))
  | (
      SignedInUserMergingIdentites({identityId}),
      Some(CollapsedAnnotationTags(annotation)),
    ) =>
    <QueryRenderers_WebView_CollapsedAnnotationTags
      identityId
      annotation
      onExpand={() => handleSetState("EDIT_ANNOTATION_TAGS")}
    />
  };
};

let page = "creators/[identityId]/webview.js";
