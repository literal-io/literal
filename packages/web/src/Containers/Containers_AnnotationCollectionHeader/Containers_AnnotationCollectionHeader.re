open Containers_AnnotationCollectionHeader_GraphQL;
open Styles;

[@react.component]
let make =
    (
      ~hideDelete=false,
      ~annotationFragment as annotation=?,
      ~annotationCollectionFragment as annotationCollection=?,
      ~onCollectionsButtonClick=?,
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
        className={cn(["pointer-events-none", "opacity-75"])}
        style={ReactDOMRe.Style.make(~width="1.5rem", ~height="1.5rem", ())}
        icon=Svg.add
      />
    </MaterialUi.IconButton>;

  let deleteButton =
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
        className={cn(["pointer-events-none", "opacity-75"])}
        style={ReactDOMRe.Style.make(~width="1.5rem", ~height="1.5rem", ())}
        icon=Svg.delete
      />
    </MaterialUi.IconButton>;

  let collectionsButton =
    <MaterialUi.IconButton
      size=`Small
      edge=MaterialUi.IconButton.Edge._end
      onClick={_ => onCollectionsButtonClick->Belt.Option.forEach(cb => cb())}
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
        className={cn(["pointer-events-none"])}
        style={ReactDOMRe.Style.make(~width="1.5rem", ~height="1.5rem", ())}
        icon=Svg.waves
      />
    </MaterialUi.IconButton>;

  <>
    <Header
      className={cn([
        "absolute",
        "left-0",
        "right-0",
        "top-0",
        "bg-darkAccent",
        "z-10",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
        "flex",
        "flex-row",
        "flex-1",
        "justify-between",
        "h-14",
        "px-4",
        "max-w-full",
      ])}>
      <div
        className={Cn.fromList([
          "flex",
          "items-center",
          "flex-shrink",
          "overflow-x-auto"
        ])}>
        collectionsButton
        {annotationCollection
         ->Belt.Option.map(annotationCollection => {
             let type_ =
               annotationCollection##type_
               ->Belt.Array.getBy(t =>
                   switch (t) {
                   | `TAG_COLLECTION
                   | `SOURCE_COLLECTION => true
                   | _ => false
                   }
                 )
               ->Belt.Option.getWithDefault(`TAG_COLLECTION);

             let (label, icon) =
               switch (type_) {
               | `SOURCE_COLLECTION => (
                   annotationCollection##label
                   ->Webapi.Url.make
                   ->Webapi.Url.host,
                   Svg.article,
                 )
               | _ => (annotationCollection##label, Svg.label)
               };

             <div
               className={Cn.fromList([
                 "flex",
                 "flex-grow-0",
                 "overflow-hidden",
                 "items-center",
                 "ml-6",
               ])}>
               <Svg
                 placeholderViewBox="0 0 24 24"
                 className={Cn.fromList([
                   "pointer-events-none",
                   "opacity-75",
                 ])}
                 style={ReactDOMRe.Style.make(
                   ~width="1rem",
                   ~height="1rem",
                   (),
                 )}
                 icon
               />
               <span
                 className={Cn.fromList([
                   "ml-2",
                   "block",
                   "font-sans",
                   "text-lightPrimary",
                   "whitespace-no-wrap",
                   "overflow-x-hidden",
                   "truncate",
                   "font-bold",
                   "text-lg",
                 ])}>
                 {React.string(label)}
               </span>
             </div>;
           })
         ->Belt.Option.getWithDefault(
             <Skeleton
               variant=`text
               className={Cn.fromList([
                 "h-4",
                 "w-32",
                 "ml-6",
                 "transform-none",
               ])}
             />,
           )}
      </div>
      <div className={Cn.fromList(["flex", "items-center", "flex-shrink-0"])}>
        {hideDelete ? React.null : deleteButton}
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
    </Header>
  </>;
};
