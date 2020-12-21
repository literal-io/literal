open Containers_Onboarding_GraphQL;
open Containers_Onboarding_Apollo;

let onboardingNotes = [|
  (
    "Welcome to Literal.\n\nLiteral is an annotation management system. Annotations from books, text, and articles that you read will appear here.\n\nSwipe left to learn more.",
    None,
  ),
  (
    "Literal is best used to capture annotations while you're reading. Literal is agnostic about where and how you read text, and supports annotations made within a web browser, PDF reader, and more.\n\nTo create an annotation, highlight the text and use your device's share dialog to share it to the Literal application. If there is no share dialog, screenshot the highlight and share the screenshot to the Literal application. You can also tap the \"+\" icon in the upper right hand corner of the screen to manually create an annotation.\n\nSwipe left to learn more.",
    None,
  ),
  (
    "Annotations are organized primarily based on tags and bi-directional links between tags in order to retain context and build connections.\n\nTo add a tag to an annotation, write text into the input at the bottom of the screen and tap your device's \"enter\" key. Tags appear in a list at the bottom of an annotation.\n\nTap on a tag assocated with this annotation and swipe left to explore.\n\n If you have any questions, feel free to reach out to hello@literal.io. Once you've created some annotations, feel free to delete these introductory example annotations.",
    Some([|"knowledge"|]),
  ),
  (
    "With no effort, he had learned English, French, Portuguese and Latin. I suspect, however, that he was not very capable of thought. To think is to forget differences, generalize, make abstractions. In the teeming world of Funes, there were only details, almost immediate in their presence.",
    Some([|"jorge luis borges", "knowledge"|]),
  ),
  (
    "I have fought sixty battles and I have learned nothing which I did not know at the beginning. Look at Caesar; he fought the first like the last.",
    Some([|"napoleon", "knowledge"|]),
  ),
  (
    "Many intelligence reports in war are contradictory; even more are false, and most are uncertain... In short, most intelligence is false.",
    Some([|"carl von clausewitz", "knowledge"|]),
  ),
|];

[@react.component]
let make = (~currentUser, ~onAnnotationIdChange) => {
  let (onboardingMutation, _s, _f) =
    ApolloHooks.useMutation(OnboardingMutation.definition);

  let _ =
    React.useEffect0(() => {
      /** enforce desired ordering of onboarding highlights via createdAt timestamps */
      let baseTs = Js.Date.make();
      let createAnnotationInputs =
        onboardingNotes
        ->Belt.Array.reverse
        ->Belt.Array.mapWithIndex((idx, (text, tags)) => {
            let _ = baseTs->Js.Date.setMilliseconds(float_of_int(idx));
            let ts = baseTs->Js.Date.toISOString;
            let tagsWithIdsP =
              tags
              ->Belt.Option.getWithDefault([||])
              ->Belt.Array.map(tag =>
                  Lib_GraphQL.AnnotationCollection.makeId(
                    ~creatorUsername=
                      AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                    ~label=tag,
                  )
                  |> Js.Promise.then_(id =>
                       Js.Promise.resolve(
                         Containers_AnnotationEditor_Types.{
                           id: Some(id),
                           text: tag,
                           href: None,
                         },
                       )
                     )
                )
              ->Belt.Array.concat([|
                  Js.Promise.resolve(
                    Containers_AnnotationEditor_Types.{
                      id:
                        Some(
                          Lib_GraphQL.AnnotationCollection.(
                            makeIdFromComponent(
                              ~creatorUsername=
                                currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                              ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
                              (),
                            )
                          ),
                        ),
                      text: "recent",
                      href: None,
                    },
                  ),
                |])
              ->Js.Promise.all;

            let annotationIdP =
              Lib_GraphQL.Annotation.makeId(
                ~creatorUsername=
                  AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                ~textualTargetValue=text,
              );

            Js.Promise.all2((annotationIdP, tagsWithIdsP))
            |> Js.Promise.then_(((annotationId, tagsWithIds)) =>
                 Js.Promise.resolve({
                   "context": [|Lib_GraphQL.Annotation.defaultContext|],
                   "type": [|`ANNOTATION|],
                   "id": annotationId,
                   "body":
                     tagsWithIds
                     ->Belt.Array.map(
                         (Containers_AnnotationEditor_Types.{text, id}) =>
                         {
                           "textualBody":
                             Some({
                               "id": id,
                               "value": text,
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
                         }
                       )
                     ->Js.Option.some,
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

      let createAgentInput = {
        let username = AwsAmplify.Auth.CurrentUserInfo.(currentUser->username);
        let email =
          AwsAmplify.Auth.CurrentUserInfo.(currentUser->attributes->email);

        Lib_GraphQL.makeHash(~digest="SHA-1", email)
        |> Js.Promise.then_(hashedEmail =>
             Js.Promise.resolve({
               "id": Constants.apiOrigin ++ "/agents/" ++ username,
               "email": Some([|Some(email)|]),
               "email_sha1": Some([|Some(hashedEmail)|]),
               "type": `PERSON,
               "username": username,
               "homepage": None,
               "name": None,
               "nickname": None,
             })
           );
      };

      let _ =
        Js.Promise.all2((
          createAgentInput,
          Js.Promise.all(createAnnotationInputs),
        ))
        |> Js.Promise.then_(((createAgentInput, createAnnotationInputs)) => {
             let variables =
               OnboardingMutation.makeVariables(
                 ~createAgentInput,
                 ~createAnnotationInput1=createAnnotationInputs[0],
                 ~createAnnotationInput2=createAnnotationInputs[1],
                 ~createAnnotationInput3=createAnnotationInputs[2],
                 ~createAnnotationInput4=createAnnotationInputs[3],
                 ~createAnnotationInput5=createAnnotationInputs[4],
                 ~createAnnotationInput6=createAnnotationInputs[5],
                 (),
               );
             let result = onboardingMutation(~variables, ());
             let _ = updateCache(~currentUser, ~createAnnotationInputs);
             result;
           })
        |> Js.Promise.then_(((mutationResult, _)) => {
             switch (mutationResult) {
             | ApolloHooks.Mutation.Errors(errors) => ()
             | NoData => Error.(report(ApolloEmptyData))
             | Data(_) => ()
             };
             Js.Promise.resolve();
           });

      None;
    });

  <Loading />;
};
