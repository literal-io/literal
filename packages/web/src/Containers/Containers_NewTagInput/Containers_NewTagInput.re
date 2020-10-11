open Containers_NewTagInput_GraphQL;

[@react.component]
let make = (~currentUser, ~annotationFragment as annotation, ~disabled=?) => {
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
           let commitBodyInput = {
             "textualBody":
               Some({
                 "id": Some(id),
                 "value": value,
                 "purpose": Some([|`TAGGING|]),
                 "rights": None,
                 "accessibility": None,
                 "format": Some(`TEXT_PLAIN),
                 "textDirection": Some(`LTR),
                 "language": Some(`EN_US),
                 "processingLanguage": Some(`EN_US),
                 "type": Some(`TEXTUAL_BODY),
               }),
             "externalBody": None,
             "choiceBody": None,
             "specificBody": None,
           };
           let existingBodyInputs =
             annotation##body
             ->Belt.Option.getWithDefault([||])
             ->Belt.Array.keepMap(body =>
                 switch (body) {
                 | `Nonexhaustive => None
                 | `TextualBody(body) =>
                   Some({
                     "textualBody":
                       Some({
                         "id": body##id,
                         "format": body##format,
                         "processingLanguage": body##processingLanguage,
                         "language": body##language,
                         "textDirection": body##textDirection,
                         "accessibility": body##accessibility,
                         "rights": body##rights,
                         "purpose": body##purpose,
                         "value": body##value,
                         "type": Some(`TEXTUAL_BODY),
                       }),
                     "externalBody": None,
                     "choiceBody": None,
                     "specificBody": None,
                   })
                 }
               );
           let input = {
             "id": annotation##id,
             "creatorUsername":
               AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
             "operations": [|
               {
                 "set":
                   Some({
                     "body":
                       Some(
                         Js.Array2.concat(
                           [|commitBodyInput|],
                           existingBodyInputs,
                         ),
                       ),
                     "target": None,
                   }),
               },
             |],
           };
           let variables = PatchAnnotationMutation.makeVariables(~input, ());
           let _ =
             Containers_NewTagInput_Apollo.updateCache(
               ~annotation,
               ~currentUser,
               ~tag=commitBodyInput##textualBody->Belt.Option.getExn,
               ~patchAnnotationMutationInput=input
             );
           let _ = setPendingValue(_ => "");
           patchAnnotationMutation(~variables, ());
         });
    ();
  };

  <TextInput_Tags
    className={Cn.fromList([
      "absolute",
      "bottom-0",
      "left-0",
      "right-0",
      "px-2",
    ])}
    ?disabled
    onValueChange=handleValueChange
    onValueCommit=handleValueCommit
    value=pendingValue
  />;
};
