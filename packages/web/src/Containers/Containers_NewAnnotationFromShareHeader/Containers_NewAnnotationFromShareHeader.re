open Styles;

let handleUpdateCache = (~currentUser, ~annotation) => {
  let _ =
    annotation##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
          Some(body)
        | _ => None
        }
      )
    ->Belt.Array.forEach(tag => {
        let cacheQuery =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=tag##id->Belt.Option.getExn,
            (),
          );
        let data =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
            ~query=cacheQuery,
            ~client=Providers_Apollo.client,
            (),
          );
        let _ =
          data
          ->Belt.Option.flatMap(d =>
              d##getAnnotationCollection->Js.Null.toOption
            )
          ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
          ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
          ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
          ->Belt.Option.forEach(items => {
              let newItems =
                items
                ->Belt.Array.keep(d => d##annotation##id != annotation##id)
                ->Js.Null.return;

              let newData =
                QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
                  data->Js.Option.getExn,
                  newItems,
                );
              let _ =
                QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
                  ~query=cacheQuery,
                  ~client=Providers_Apollo.client,
                  ~data=newData,
                  (),
                );
              ();
            });

        ();
      });
  ();
};

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
          ~input={
            "id": annotation##id,
            "creatorUsername":
              AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
          },
          (),
        );

      let _ = handleUpdateCache(~annotation, ~currentUser);
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
      let _ = Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ())));
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
