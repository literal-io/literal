open Styles;

let handleUpdateCache = (~currentUser, ~annotation) => {
  let cacheQuery =
    QueryRenderers_Annotations_GraphQL.ListAnnotations.Query.make(
      ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      (),
    );
  let _ =
    QueryRenderers_Annotations_GraphQL.ListAnnotations.readCache(
      ~query=cacheQuery,
      ~client=Providers_Apollo.client,
      (),
    )
    ->Belt.Option.flatMap(cachedQuery => cachedQuery##listAnnotations)
    ->Belt.Option.flatMap(listAnnotations => listAnnotations##items)
    ->Belt.Option.forEach(annotations => {
        let newAnnotations =
          annotations->Belt.Array.keep(cachedAnnotation =>
            switch (cachedAnnotation) {
            | Some(cachedAnnotation) =>
              cachedAnnotation##id !== annotation##id
            | None => false
            }
          );
        let newData = {
          "listAnnotations":
            Some({
              "__typename": "ModelAnnotationConnection",
              "items": Some(newAnnotations),
            }),
          "__typename": "Query",
        };
        let _ =
          QueryRenderers_Annotations_GraphQL.ListAnnotations.writeCache(
            ~query=cacheQuery,
            ~client=Providers_Apollo.client,
            ~data=newData,
            (),
          );
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

      let _ =
        deleteAnnotationMutation(~variables, ())
        |> Js.Promise.then_(_ => {
             let _ = handleUpdateCache(~annotation, ~currentUser);
             let _ =
               Webview.(
                 postMessage(WebEvent.make(~type_="ACTIVITY_FINISH"))
               );
             Js.Promise.resolve();
           });
      ();
    | _ =>
      let _ = Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH")));
      ();
    };
    ();
  };

  <Header className={cn(["py-2", "mx-6"])}>
    <MaterialUi.IconButton
      size=`Small
      edge=MaterialUi.IconButton.Edge.start
      onClick={_ => handleClose()}
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
