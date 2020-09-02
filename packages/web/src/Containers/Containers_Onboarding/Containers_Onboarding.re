open Containers_Onboarding_GraphQL;

let onboardingNotes = [|
  "Welcome to Literal.\n\nLiteral is an annotation management system. Annotations from books that you read will appear here.\n\nScroll right to learn more.",
  "Literal is best used to capture annotations while you're reading. Literal is agnostic about where and how you currently read text, and supports annotations made within a web browser, PDF reader, and more.\n\nTo create a note, highlight the text and use your device's share dialog to share it to the Literal application. If there is no share dialog, screenshot the highlight and share the screenshot to the Literal application.\n\nScroll right to learn more.",
  "Annotations are organized primarily based on tags and bi-directional links between tags in order to retain context and build connections.\n\nIf you have any questions, reach out to hello@literal.io.\n\nOnce you've created some annotations, feel free to delete these introductory example annotations.",
|];

let handleUpdateCache = (~currentUser, ~mutationData) =>
  /** FIXME
  let cacheQuery =
    QueryRenderers_Notes_GraphQL.ListHighlights.Query.make(
      ~owner=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
      (),
    );
  /** FIXME: add tags **/
  let updatedItems =
    Belt.Array.map(
      [|
        mutationData##createHighlight1,
        mutationData##createHighlight2,
        mutationData##createHighlight3,
      |],
      fun
      | Some(highlight) =>
        Some(
          QueryRenderers_Notes_GraphQL.ListHighlights.Raw.{
            id: highlight##id,
            createdAt: highlight##createdAt,
            text: highlight##editorHighlightFragment##text,
            typename: highlight##__typename,
            tags: None,
          },
        )
      | None => None,
    );

  let _ =
    switch (
      QueryRenderers_Notes_GraphQL.ListHighlights.readCache(
        ~client=Provider.client,
        ~query=cacheQuery,
        (),
      )
    ) {
    | None => ()
    | Some(cachedQuery) =>
      let updatedListHighlights =
        QueryRenderers_Notes_GraphQL.ListHighlights.Raw.(
          cachedQuery
          ->listHighlights
          ->Belt.Option.flatMap(highlightConnectionItems)
          ->Belt.Option.map(items => {
              {
                ...cachedQuery,
                listHighlights:
                  Some({
                    ...cachedQuery->listHighlights->Belt.Option.getExn,
                    items: Some(Belt.Array.concat(items, updatedItems)),
                  }),
              }
            })
        );
      let _ =
        switch (updatedListHighlights) {
        | Some(updatedListHighlights) =>
          QueryRenderers_Notes_GraphQL.ListHighlights.(
            writeCache(
              ~client=Provider.client,
              ~data=updatedListHighlights,
              ~query=cacheQuery,
              (),
            )
          )
        | None => ()
        };
      ();
    };
  **/
  {
    ();
  };

[@react.component]
let make = (~currentUser) => {
  let (onboardingMutation, _s, _f) =
    ApolloHooks.useMutation(OnboardingMutation.definition);

  let _ =
    React.useEffect0(() => {
      /** enforce desired ordering of onboarding highlights via createdAt timestamps */
      let baseTs = Js.Date.make();
      let createAnnotationInputs =
        onboardingNotes
        ->Belt.Array.reverse
        ->Belt.Array.mapWithIndex((idx, text) => {
            let _ = baseTs->Js.Date.setMilliseconds(float_of_int(idx));
            let ts = baseTs->Js.Date.toISOString;
            Lib_GraphQL.Annotation.makeId(
              ~creatorUsername=
                AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
              ~textualTargetValue=text,
            )
            |> Js.Promise.then_(id =>
                 Js.Promise.resolve({
                   "context": [|Lib_GraphQL.Annotation.defaultContext|],
                   "type": [|`ANNOTATION|],
                   "id": id,
                   "body": None,
                   "created": Some(ts->Js.Json.string),
                   "modified": Some(ts->Js.Json.string),
                   "generated": Some(ts->Js.Json.string),
                   "audience": None,
                   "canonical": None,
                   "stylesheet": None,
                   "via": None,
                   "motivation": Some([|`HIGHLIGHTING|]),
                   "creatorUsername":
                     AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                   "annotationGeneratorId": None,
                   "target": [|
                     {
                       "externalTarget": None,
                       "textualTarget":
                         Some({
                           "format": Some(`TEXT_PLAIN),
                           "language": Some(`EN_US),
                           "processingLanguage": Some(`EN_US),
                           "textDirection": Some(`LTR),
                           "accessibility": None,
                           "rights": None,
                           "value": text,
                           "id": None,
                         }),
                     },
                   |],
                 })
               );
          });

      let _ =
        Js.Promise.all(createAnnotationInputs)
        |> Js.Promise.then_(inputs => {
             let variables =
               OnboardingMutation.makeVariables(
                 ~createAnnotationInput1=inputs[0],
                 ~createAnnotationInput2=inputs[1],
                 ~createAnnotationInput3=inputs[2],
                 (),
               );
             onboardingMutation(~variables, ());
           })
        |> Js.Promise.then_(((mutationResult, _)) => {
             switch (mutationResult) {
             | ApolloHooks.Mutation.Errors(errors) =>
               errors->Belt.Array.forEach(error => {
                 Error.(report(GraphQLError(error)))
               })
             | Data(mutationData) =>
               let _ = handleUpdateCache(~currentUser, ~mutationData);
               ();
             | NoData => Error.(report(ApolloEmptyData))
             };
             Js.Promise.resolve();
           });
      None;
    });

  <Loading />;
};
