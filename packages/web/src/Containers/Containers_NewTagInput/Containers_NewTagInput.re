open Containers_NewTagInput_GraphQL;

module Loading = {
  [@react.component]
  let make = () => {
    let noop = _ => ();

    <TextInput_Tags
      className={Cn.fromList([
        "absolute",
        "bottom-0",
        "left-0",
        "right-0",
        "px-2",
      ])}
      disabled=true
      onValueChange=noop
      onValueCommit=noop
      value=""
    />;
  };
};

module Data = {
  [@react.component]
  let make = (~currentUser, ~annotation) => {
    let (patchAnnotationMutation, _s, _f) =
      ApolloHooks.useMutation(PatchAnnotationMutation.definition);
    let (pendingValue, setPendingValue) = React.useState(_ => "");

    let handleValueChange = value => setPendingValue(_ => value);
    let handleValueCommit = value => {
      let _ =
        Lib_GraphQL.AnnotationCollection.makeId(
          ~creatorUsername=
            AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
          ~label=value,
        )
        |> Js.Promise.then_(id => {
             let operation =
               Lib_GraphQL_PatchAnnotationMutation.Input.makeOperation(
                 ~add=
                   Lib_GraphQL_PatchAnnotationMutation.Input.makeAdd(
                     ~body=
                       Lib_GraphQL_AnnotationBodyInput.makeBody(
                         ~textualBody=
                           Lib_GraphQL_AnnotationBodyInput.makeTextualBody(
                             ~id,
                             ~value,
                             ~purpose=[|`TAGGING|],
                             ~format=`TEXT_PLAIN,
                             ~textDirection=`LTR,
                             ~language=`EN_US,
                             ~processingLanguage=`EN_US,
                             (),
                           ),
                         (),
                       ),
                     (),
                   ),
                 (),
               );
             let input =
               Lib_GraphQL_PatchAnnotationMutation.Input.make(
                 ~id=annotation##id,
                 ~creatorUsername=
                   AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                 ~operations=[|operation|],
               );
             let variables =
               PatchAnnotationMutation.makeVariables(~input, ());
             let _ =
               Lib_GraphQL_PatchAnnotationMutation.Apollo.updateCache(
                 ~currentUser,
                 ~input,
               );
             let _ = setPendingValue(_ => "");
             patchAnnotationMutation(~variables, ());
           });
      ();
    };

    <TextInput_Tags
      className={Cn.fromList([
        "z-10",
        "absolute",
        "bottom-0",
        "left-0",
        "right-0",
        "px-2",
        "bg-black",
      ])}
      onValueChange=handleValueChange
      onValueCommit=handleValueCommit
      value=pendingValue
    />;
  };
};

[@react.component]
let make = (~currentUser=?, ~annotationFragment as annotation=?) => {
  switch (currentUser, annotation) {
  | (Some(currentUser), Some(annotation)) => <Data currentUser annotation />
  | _ => <Loading />
  };
};
