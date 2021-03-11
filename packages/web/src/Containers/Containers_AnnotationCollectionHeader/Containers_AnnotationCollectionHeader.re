open Containers_AnnotationCollectionHeader_GraphQL;
open Styles;

[@react.component]
let make =
    (
      ~annotationFragment as annotation=?,
      ~annotationCollectionFragment as annotationCollection=?,
      ~currentUser=?,
    ) => {
  let (deleteAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_AnnotationCollectionHeader_GraphQL.DeleteAnnotationMutation.definition,
    );

  let handleDelete = (~annotation, ~currentUser) => {
    let input =
      Lib_GraphQL_DeleteAnnotationMutation.Input.make(
        ~creatorUsername=
          AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
        ~id=annotation##id,
      );
    let variables = DeleteAnnotationMutation.makeVariables(~input, ());

    let _ = deleteAnnotationMutation(~variables, ());
    let _ =
      Lib_GraphQL_DeleteAnnotationMutation.Apollo.updateCache(
        ~annotation,
        ~currentUser,
      );
    ();
  };

  let createButton =
    <MaterialUi.IconButton
      size=`Small
      onClick={_ => {
        let _ =
          Service_Analytics.(track(Click({action: "create", label: None})));
        ();
      }}
      edge=MaterialUi.IconButton.Edge._end
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(
        ~root=cn(["p-0", "ml-4"]),
        (),
      )}>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["pointer-events-none", "opacity-50"])}
        style={ReactDOMRe.Style.make(~width="1.75rem", ~height="1.75rem", ())}
        icon=Svg.add
      />
    </MaterialUi.IconButton>;

  <Header
    className={cn([
      "absolute",
      "left-0",
      "right-0",
      "top-0",
      "bg-black",
      "z-10",
    ])}>
    <div
      className={cn([
        "justify-between",
        "items-center",
        "py-2",
        "mx-4",
        "flex",
        "flex-1",
      ])}>
      <h1
        className={cn([
          "text-lightPrimary",
          "font-sans",
          "font-bold",
          "leading-none",
          "text-lg",
        ])}>
        {annotationCollection
         ->Belt.Option.map(ac => React.string(ac##label))
         ->Belt.Option.getWithDefault(React.null)}
      </h1>
      <div className={cn(["flex", "flex-row"])}>
        <MaterialUi.IconButton
          size=`Small
          edge=MaterialUi.IconButton.Edge._end
          onClick={_ =>
            switch (annotation, currentUser) {
            | (Some(annotation), Some(currentUser)) =>
              let _ =
                Service_Analytics.(
                  track(Click({action: "delete", label: None}))
                );
              handleDelete(~annotation, ~currentUser);
            | _ => ()
            }
          }
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-50"]),
            },
          }
          classes={MaterialUi.IconButton.Classes.make(
            ~root=cn(["p-0", "ml-1"]),
            (),
          )}>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["pointer-events-none", "opacity-50"])}
            style={ReactDOMRe.Style.make(
              ~width="1.75rem",
              ~height="1.75rem",
              (),
            )}
            icon=Svg.delete
          />
        </MaterialUi.IconButton>
        {switch (currentUser) {
         | Some(currentUser) =>
           <Next.Link
             _as={Routes.CreatorsIdAnnotationsNew.path(
               ~creatorUsername=
                 currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
             )}
             href=Routes.CreatorsIdAnnotationsNew.staticPath>
             createButton
           </Next.Link>
         | _ => createButton
         }}
      </div>
    </div>
  </Header>;
};
