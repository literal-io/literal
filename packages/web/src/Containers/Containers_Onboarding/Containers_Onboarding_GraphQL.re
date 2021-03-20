module GetAgentFragment = [%graphql
  {|
    fragment onboardingAgentFragment on Agent {
      id
      username
    }
  |}
];

module OnboardingMutation = [%graphql
  {|
    mutation onboarding(
      $createAgentInput: CreateAgentInput!
      $createAnnotationInput1: CreateAnnotationInput!
      $createAnnotationInput2: CreateAnnotationInput!
      $createAnnotationInput3: CreateAnnotationInput!
      $createAnnotationInput4: CreateAnnotationInput!
      $createAnnotationInput5: CreateAnnotationInput!
      $createAnnotationInput6: CreateAnnotationInput!
    ) {
      createAgent(input: $createAgentInput) {
        id
      }
      createAnnotation1: createAnnotation(input: $createAnnotationInput1) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
      createAnnotation2: createAnnotation(input: $createAnnotationInput2) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
      createAnnotation3: createAnnotation(input: $createAnnotationInput3) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
      createAnnotation4: createAnnotation(input: $createAnnotationInput4) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
      createAnnotation5: createAnnotation(input: $createAnnotationInput5) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
      createAnnotation6: createAnnotation(input: $createAnnotationInput6) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
    }
  |}
];

let makeOnboardingAnnotation =
    (
      ~textualTargetValue,
      ~currentUser,
      ~timestamp,
      ~textualBodyValues=?,
      ~specificTarget=?,
      (),
    ) => {
  textualBodyValues
  ->Belt.Option.getWithDefault([||])
  ->Belt.Array.map(tag =>
      Lib_GraphQL.AnnotationCollection.makeId(
        ~creatorUsername=
          AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
        ~label=tag,
      )
      |> Js.Promise.then_(id =>
           Js.Promise.resolve(
             Containers_AnnotationEditor_Tag.{
               id: Some(id),
               text: tag,
               href: None,
             },
           )
         )
    )
  ->Belt.Array.concat([|
      Js.Promise.resolve(
        Containers_AnnotationEditor_Tag.{
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
  ->Js.Promise.all
  |> Js.Promise.then_(tagsWithIds => {
       let body =
         tagsWithIds->Belt.Array.map(
           (Containers_AnnotationEditor_Tag.{text, id}) =>
           Lib_GraphQL_AnnotationBodyInput.(
             makeBody(
               ~textualBody=
                 makeTextualBody(
                   ~id=id->Belt.Option.getExn,
                   ~value=text,
                   ~purpose=[|`TAGGING|],
                   ~format=`TEXT_PLAIN,
                   ~textDirection=`LTR,
                   ~language=`EN_US,
                   ~processingLanguage=`EN_US,
                   (),
                 ),
               (),
             )
           )
         );
       let target =
         [|
           Some(
             Lib_GraphQL_AnnotationTargetInput.(
               make(
                 ~textualTarget=
                   makeTextualTarget(
                     ~id=Uuid.makeV4(),
                     ~value=textualTargetValue,
                     ~textDirection=`LTR,
                     ~processingLanguage=`EN_US,
                     ~language=`EN_US,
                     ~format=`TEXT_PLAIN,
                     (),
                   ),
                 (),
               )
             ),
           ),
           specificTarget,
         |]
         ->Belt.Array.keepMap(i => i);

       Lib_GraphQL_CreateAnnotationMutation.Input.make(
         ~context=[|Lib_GraphQL.Annotation.defaultContext|],
         ~id=Uuid.makeV4(),
         ~body,
         ~created=timestamp->Js.Json.string,
         ~modified=timestamp->Js.Json.string,
         ~generated=timestamp->Js.Json.string,
         ~motivation=[|`HIGHLIGHTING|],
         ~creatorUsername=
           AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
         ~target,
         (),
       )
       ->Js.Promise.resolve;
     });
};

/**
let annotations = [|
  makeOnboardingAnnotation(
    ~textualTargetValue=
      "Welcome to Literal.\n\nLiteral is a textual annotation management system; a tool for bookmarking thoughts, ideas, and knowledge.\n\nAs you read, capture highlights of text with Literal. Literal automatically archives the source, context, and location of your annotation. Use tags to organize and add notes to annotations. Over time, revisit and leverage your annotations and their source context to augment the reading experience.\n\nSwipe left to learn more. ->",
  ),
  makeOnboardingAnnotation(
    ~textualTargetValue=
      "Capture annotations wherever you read.\n\nTo annotate web-based content, use the device sharesheet to share the URL with Literal, and use the source viewer to read and annotate.\n\nTo annotate other content types (e.g. PDF, Kindle), highlight the text within your reader application, take a screenshot, and use the device sharesheet to share the screenshot with Literal.\n\nSwipe left to learn more. ->",
  ),
  makeOnboardingAnnotation(
    ~textualTargetValue=
      "Annotations are organized by tags and their source context. Tap the \"knowledge\" tag associated with this annotation and swipe left to explore example annotations. Tap an example annotation to see its source.\n\nIf you have any questions, feel to reach out to daniel@literal.io. Once you\'ve created annotations of your own, feel free to delete these example anotations.",
    ~textualBodyValues=[|"knowledge"|],
  ),
  makeOnboardingAnnotation(
    ~textualTargetValue=
      "The Library exists ab aeterno. This truth, whose immediate corollary is the future eternity of the world, cannot be placed in doubt by any reasonable mind. Man, the imperfect librarian, may be the product of chance or of malevolent demiurgi; the universe, with its elegant endowment of shelves, of enigmatical volumes, of inexhaustible stairways for the traveler and latrines for the seated librarian, can only be the work of a god. To perceive the distance between the divine and the human, it is enough to compare these crude wavering symbols which my fallible hand scrawls on the cover of a book, with the organic letters inside: punctual, delicate, perfectly black, inimitably symmetrical.",
    ~specificTarget=
      Lib_GraphQL_AnnotationTargetInput.make(
        ~specificTarget=
          Lib_GraphQL_AnnotationTargetInput.makeSpecificTarget(
            ~source=
              Lib_GraphQL_AnnotationTargetInput.make(
                ~externalTarget=
                  Lib_GraphQL_AnnotationTargetInput.makeExternalTarget(
                    ~id="https://urbigenous.net/library/library_of_babel.html",
                    ~hashId=
                      "56afdd38e2719fd3728c8f47df65761ebc306225d50eef13963ba32cc04b63ad",
                    ~format=`TEXT_HTML,
                    ~language=`EN_US,
                    ~processingLanguage=`EN_US,
                    ~textDirection=`LTR,
                    ~type_=`TEXT,
                    (),
                  ),
              ),
            ~selector=[|
              Lib_GraphQL_SelectorInput.make(
                ~rangeSelector=
                  Lib_GraphQL_SelectorInput.makeRangeSelectorInput(
                    ~startSelector=
                      Lib_GraphQL_SelectorInput.make(
                        ~xPathSelector=
                          makeXPathSelectorInput(
                            ~value="/html/body/main/p[4]/text()[1]",
                            ~refinedBy=[|
                              Lib_GraphQL_SelectorInput.make(
                                ~textPositionSelector=
                                  Lib_GraphQL_SelectorInput.makeTextPositionSelectorInput(
                                    ~start=8,
                                    ~end_=27,
                                    (),
                                  ),
                                (),
                              ),
                            |],
                            (),
                          ),
                        (),
                      ),
                    ~endSelector=
                      Lib_GraphQL_SelectorInput.make(
                        ~xPathSelector=
                          makeXPathSelectorInput(
                            ~value="/html/body/main/p[4]/text()[2]",
                            ~refinedBy=[|
                              Lib_GraphQL_SelectorInput.make(
                                ~textPositionSelector=
                                  Lib_GraphQL_SelectorInput.makeTextPositionSelectorInput(
                                    ~start=0,
                                    ~end_=663,
                                    (),
                                  ),
                                (),
                              ),
                            |],
                            (),
                          ),
                        (),
                      ),
                    (),
                  ),
                (),
              ),
            |],
          ),
      ),
  ),
|];
**/
