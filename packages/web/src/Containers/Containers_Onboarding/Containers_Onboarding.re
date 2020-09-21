open Containers_Onboarding_GraphQL;

let onboardingNotes = [|
  "Welcome to Literal.\n\nLiteral is an annotation management system. Annotations from books that you read will appear here.\n\nScroll right to learn more.",
  "Literal is best used to capture annotations while you're reading. Literal is agnostic about where and how you currently read text, and supports annotations made within a web browser, PDF reader, and more.\n\nTo create a note, highlight the text and use your device's share dialog to share it to the Literal application. If there is no share dialog, screenshot the highlight and share the screenshot to the Literal application.\n\nScroll right to learn more.",
  "Annotations are organized primarily based on tags and bi-directional links between tags in order to retain context and build connections.\n\nIf you have any questions, reach out to hello@literal.io.\n\nOnce you've created some annotations, feel free to delete these introductory example annotations.",
|];

let updateCache = (~currentUser, ~createAnnotationInputs) => {
  let _ =
    createAnnotationInputs
    ->Belt.Array.map(annotationInput =>
        annotationInput
        ->OnboardingMutation.json_of_CreateAnnotationInput
        ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput
      )
    ->Belt.Array.map(annotation =>
        annotation##body
        ->Js.Null.toOption
        ->Belt.Option.getWithDefault([||])
        ->Belt.Array.keepMap(body => {
            let isTag =
              body##purpose
              ->Js.Null.toOption
              ->Belt.Option.map(d => d->Belt.Array.some(p => p == "TAGGING"))
              ->Belt.Option.getWithDefault(false)
              &&
              body##__typename == "TextualBody"
              && body##id->Js.Null.toOption->Belt.Option.isSome;
            isTag ? Some((annotation, body)) : None;
          })
      )
    ->Belt.Array.concatMany
    ->Belt.Array.forEach(((annotation, tag)) => {
        let cacheQuery =
          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~id=tag##id->Js.Null.toOption->Belt.Option.getExn,
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
              Belt.Array.concat(
                [|
                  {
                    "__typename": "AnnotationPageItem",
                    "annotation": annotation,
                  },
                |],
                items,
              );

            QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setCacheItems(
              data,
              newItems,
            );
          | None => {
              "__typename": "Query",
              "getAnnotationCollection":
                Js.Null.return({
                  "__typename": "AnnotationCollection",
                  "label": tag##value,
                  "first":
                    Js.Null.return({
                      "__typename": "AnnotationPage",
                      "items":
                        Js.Null.return({
                          "__typename": "ModelAnnotationPageItemConnection",
                          "items":
                            Js.Null.return([|
                              {
                                "__typename": "AnnotationPageItem",
                                "annotation": annotation,
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
      });
  ();
};

let updateCache = (~currentUser, ~createAnnotationInputs) => {
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
          createAnnotationInputs->Belt.Array.map(annotationInput =>
            annotationInput
            ->OnboardingMutation.json_of_CreateAnnotationInput
            ->Lib_GraphQL.Annotation.annotationFromCreateAnnotationInput
          );

        let updatedAnnotations =
          Belt.Array.concat(annotations, newAnnotations);

        let newData = {
          "listAnnotations":
            Some({
              "items": Some(updatedAnnotations),
              "__typename": "ModelAnnotationConnection",
            }),
          "__typename": "Query",
        };

        let _ =
          QueryRenderers_Annotations_GraphQL.ListAnnotations.(
            writeCache(
              ~query=cacheQuery,
              ~client=Providers_Apollo.client,
              ~data=newData,
              (),
            )
          );
        ();
      });
  ();
};

[@react.component]
let make = (~currentUser) => {
  let router = Next.Router.useRouter();
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
             let result = onboardingMutation(~variables, ());
             let _ =
               updateCache(~currentUser, ~createAnnotationInputs=inputs);
             let _ =
               Next.Router.replaceWithAs(
                 Routes.CreatorsIdAnnotationsId.staticPath,
                 Routes.CreatorsIdAnnotationsId.path(
                   ~creatorUsername=inputs[0]##creatorUsername,
                   ~annotationIdComponent=
                     Lib_GraphQL.Annotation.idComponent(inputs[0]##id),
                 ),
               );
             result;
           })
        |> Js.Promise.then_(((mutationResult, _)) => {
             switch (mutationResult) {
             | ApolloHooks.Mutation.Errors(errors) =>
               errors->Belt.Array.forEach(error => {
                 Error.(report(GraphQLError(error)))
               })
             | NoData => Error.(report(ApolloEmptyData))
             | Data(_) => ()
             };
             Js.Promise.resolve();
           });

      None;
    });

  <Loading />;
};
