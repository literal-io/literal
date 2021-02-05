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
              ~type_="EDIT_ANNOTATION_TAGS_RESULT",
              ~data=Lib_WebView_Model.Annotation.encode(newAnnotation),
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
let make =
    (
      ~rehydrated,
      ~authentication: Hooks_CurrentUserInfo_Types.state,
      ~annotation,
      ~onAnnotationChange,
    ) => {
  switch (rehydrated, authentication) {
  | (_, Unauthenticated) =>
    <Redirect
      staticPath={Routes.Authenticate.path()}
      path={Routes.Authenticate.path()}>
      <Loading />
    </Redirect>
  | (_, Loading)
  | (false, _) => <Loading />
  | (true, Authenticated(currentUser)) =>
    <Data currentUser annotation onAnnotationChange />
  };
};
