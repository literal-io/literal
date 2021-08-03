open Styles;

[@react.component]
let make = (~annotationFragment as annotation=?, ~identityId=?) => {
  let (deleteAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_NewAnnotationFromShareHeader_GraphQL.DeleteAnnotationMutation.definition,
    );

  let handleClose = () => {
    switch (annotation, identityId) {
    | (Some(annotation), Some(identityId)) =>
      let variables =
        Containers_NewAnnotationFromShareHeader_GraphQL.DeleteAnnotationMutation.makeVariables(
          ~input=
            Lib_GraphQL_DeleteAnnotationMutation.Input.make(
              ~id=annotation##id,
              ~creatorUsername=identityId,
            ),
          (),
        );

      let _ =
        Lib_GraphQL_DeleteAnnotationMutation.Apollo.updateCache(
          ~annotation,
          ~identityId,
        );
      if (!Webview.JavascriptInterface.isWebview()) {
        Next.Router.back();
      };
      let _ =
        deleteAnnotationMutation(~variables, ())
        |> Js.Promise.then_(_ => {
             let _ =
               Webview.(
                 postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ()))
               );
             Js.Promise.resolve();
           });
      ();
    | _ =>
      let _ =
        Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ())));
      ();
    };
    ();
  };

  <Header className={cn(["py-2", "mx-4"])}>
    <MaterialUi.IconButton
      size=`Small
      edge=MaterialUi.IconButton.Edge.start
      onClick={_ => {
        let _ =
          Service_Analytics.(track(Click({action: "close", label: None})));
        handleClose();
      }}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(~root=cn(["p-0"]), ())}>
      <Svg
        className={cn(["w-8", "h-8", "pointer-events-none"])}
        icon=Svg.close
      />
    </MaterialUi.IconButton>
  </Header>;
};
