open Styles;

[@react.component]
let make = (~annotationFragment as annotation=?, ~currentUser=?) => {
  let (deleteAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_NewAnnotationFromShareHeader_GraphQL.DeleteAnnotationMutation.definition,
    );

  let handleClose = () => {
    switch (annotation, currentUser) {
    | (Some(annotation), Some(currentUser)) =>
      let variables =
        Containers_NewAnnotationFromShareHeader_GraphQL.DeleteAnnotationMutation.makeVariables(
          ~input=
            Lib_GraphQL_DeleteAnnotationMutation.Input.make(
              ~id=annotation##id,
              ~creatorUsername=
                AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
            ),
          (),
        );

      let _ =
        Lib_GraphQL_DeleteAnnotationMutation.Apollo.updateCache(
          ~annotation,
          ~currentUser,
        );
      if (!Webview.isWebview()) {
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

  <Header className={cn(["py-2", "mx-6"])}>
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
        placeholderViewBox="0 0 24 24"
        className={cn(["w-8", "h-8", "pointer-events-none"])}
        icon=Svg.close
      />
    </MaterialUi.IconButton>
  </Header>;
};
