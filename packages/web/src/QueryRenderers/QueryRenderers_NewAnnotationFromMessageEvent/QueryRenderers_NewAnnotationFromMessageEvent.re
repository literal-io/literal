module AnnotationModel = QueryRenderers_NewAnnotationFromMessageEvent_AnnotationModel;

module Data = {
  [@react.component]
  let make = (~annotation, ~onAnnotationChange, ~currentUser) => {
    let handleDismiss = () => {
      let _ =
        Service_Analytics.(track(Click({action: "close", label: None})));
      let _ =
        Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ())));
      ();
    };

    let handleAnnotationChange = newAnnotation => {
      let _ = onAnnotationChange(newAnnotation);
      let _ =
        Webview.(
          postMessage(
            WebEvent.make(
              ~type_="NEW_ANNOTAITON_RESULT",
              ~data=AnnotationModel.encode(newAnnotation),
              (),
            ),
          )
        );
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
        onDismiss=handleDismiss
      />
      <Containers_NewAnnotationFromMessageEventEditor
        currentUser
        annotation
        onAnnotationChange=handleAnnotationChange
      />
    </div>;
  };
};

[@react.component]
let make = (~rehydrated, ~authentication: Hooks_CurrentUserInfo_Types.state) => {
  let (annotation, setAnnotation) = React.useState(() => None);
  let _ =
    React.useEffect0(() => {
      let handleNewAnnotation = (event: option(Js.Json.t)) => {
        event
        ->Belt.Option.map(
            QueryRenderers_NewAnnotationFromMessageEvent_AnnotationModel.decode,
          )
        ->Belt.Option.forEach(result =>
            switch (result) {
            | Ok(annotation) => setAnnotation(_ => Some(annotation))
            | Error(e) => Js.log2("error", e)
            }
          );
      };

      let _ =
        Webview.WebEventHandler.register((
          "NEW_ANNOTATION",
          handleNewAnnotation,
        ));

      Some(
        () => {
          let _ = Webview.WebEventHandler.unregister("NEW_ANNOTATION");
          ();
        },
      );
    });

  let handleAnnotationChange = newAnnotation => {
    setAnnotation(_ => Some(newAnnotation));
  };

  switch (rehydrated, authentication, annotation) {
  | (_, Unauthenticated, _) =>
    <Redirect
      staticPath={Routes.Authenticate.path()}
      path={Routes.Authenticate.path()}>
      <Loading />
    </Redirect>
  | (_, Loading, _)
  | (false, _, _)
  | (_, _, None) => <Loading />
  | (true, Authenticated(currentUser), Some(annotation)) =>
    <Data currentUser annotation onAnnotationChange=handleAnnotationChange />
  };
};
