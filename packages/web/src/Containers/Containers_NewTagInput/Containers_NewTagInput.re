open Containers_NewTagInput_GraphQL;

let handleUpdateCache = (~annotation, ~currentUser, ~tag) => {
  let cacheQuery =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
      ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      ~id=addedTag.id->Belt.Option.getExn,
      (),
    );
  let data =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
      ~query=cacheQuery,
      ~client=Providers_Apollo.client,
      (),
    );
  let newData =
    switch (data) {
    | Some(data) =>
      let items =
        data##getAnnotationCollection
        ->Js.Null.toOption
        ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
        ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
        ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
        ->Belt.Option.getWithDefault([||]);
      let newItems =
        Js.Null.return(
          Belt.Array.concat(
            [|{"__typename": "", "annotation": cacheAnnotation}|],
            items,
          ),
        );

      QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
        data,
        newItems,
      );
    | None => {
        "__typename": "Query",
        "getAnnotationCollection":
          Js.Null.return({
            "__typename": "AnnotationCollection",
            "label": addedTag.text,
            "first":
              Js.Null.return({
                "__typename": "AnnotationPage",
                "items":
                  Js.Null.return({
                    "__typename": "ModelAnnotationPageItemConnection",
                    "nextToken": Js.Null.return(""),
                    "items":
                      Js.Null.return([|
                        {
                          "__typename": "AnnotationPageItem",
                          "annotation": cacheAnnotation,
                        },
                      |]),
                  }),
              }),
          }),
      }
    };
  let _ =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
      ~query=cacheQuery,
      ~client=Providers_Apollo.client,
      ~data=newData,
      (),
    );
  ();
};

[@react.component]
let make = (~currentUser, ~annotationFragment as annotation, ~disabled=?) => {
  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(PatchAnnotationMutation.definition);
  let (pendingValue, setPendingValue) = React.useState(_ => "");

  let handleValueChange = value => setPendingValue(_ => value);
  let handleValueCommit = value => {
    Lib_GraphQL.AnnotationCollection.makeId(
      ~creatorUsername=AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
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
                   "textualBody": Some(body),
                   "externalBody": None,
                   "choiceBody": None,
                   "specificBody": None,
                 })
               }
             );
         let bodyInput =
           Js.Array2.concat([|commitBodyInput|], existingBodyInputs);
         let variables =
           PatchAnnotationMutation.makeVariables(
             ~input={
               "id": annotation##id,
               "creatorUsername":
                 AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
               "operations": [|
                 {"set": Some({"body": Some(bodyInput), "target": None})},
               |],
             },
           );
         let _ =
           handleUpdateCache(
             ~annotation,
             ~currentUser,
             ~tag=commitBodyInput##textualBody->Belt.Option.getExn,
           );
         patchAnnotationMutation(~variables, ());
       });
  };

  <TextInput_Tags
    ?disabled
    onValueChange=handleValueChange
    onValueCommit=handleValueCommit
    value=pendingValue
  />;
};
