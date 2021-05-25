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
      ~id,
      ~textualTargetValue,
      ~identityId,
      ~timestamp,
      ~textualBodyValues=?,
      ~specificTarget=?,
      (),
    ) => {
  textualBodyValues
  ->Belt.Option.getWithDefault([||])
  ->Belt.Array.map(tag =>
      Lib_GraphQL.AnnotationCollection.makeId(~identityId, ~label=tag)
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
                  ~identityId,
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
                   makeTextualTargetInput(
                     ~id=makeId(~annotationId=id),
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
         ~id,
         ~body,
         ~created=timestamp->Js.Json.string,
         ~modified=timestamp->Js.Json.string,
         ~generated=timestamp->Js.Json.string,
         ~motivation=[|`HIGHLIGHTING|],
         ~creatorUsername=identityId,
         ~target,
         (),
       )
       ->Js.Promise.resolve;
     });
};

let makeSpecificTarget =
    (
      ~annotationId,
      ~externalTargetId,
      ~externalTargetHashId,
      ~selectorStart,
      ~selectorEnd,
      ~cacheParsedUrl,
    ) => {
  let (startValue, startPosStart, startPosEnd) = selectorStart;
  let (endValue, endPosStart, endPosEnd) = selectorEnd;

  Lib_GraphQL_AnnotationTargetInput.make(
    ~specificTarget=
      Lib_GraphQL_AnnotationTargetInput.makeSpecificTargetInput(
        ~id=Lib_GraphQL_AnnotationTargetInput.makeId(~annotationId),
        ~source=
          Lib_GraphQL_AnnotationTargetInput.make(
            ~externalTarget=
              Lib_GraphQL_AnnotationTargetInput.makeExternalTargetInput(
                ~id=externalTargetId,
                ~hashId=externalTargetHashId,
                ~format=`TEXT_HTML,
                ~language=`EN_US,
                ~processingLanguage=`EN_US,
                ~textDirection=`LTR,
                ~type_=`TEXT,
                (),
              ),
            (),
          ),
        ~state=[|
          Lib_GraphQL_StateInput.make(
            ~timeState=
              Lib_GraphQL_StateInput.makeTimeStateInput(
                ~sourceDate=[|Js.Date.(make()->toISOString)->Js.Json.string|],
                ~cached=[|cacheParsedUrl|],
                (),
              ),
            (),
          ),
        |],
        ~selector=[|
          Lib_GraphQL_SelectorInput.make(
            ~rangeSelector=
              Lib_GraphQL_SelectorInput.makeRangeSelectorInput(
                ~startSelector=
                  Lib_GraphQL_SelectorInput.make(
                    ~xPathSelector=
                      Lib_GraphQL_SelectorInput.makeXPathSelectorInput(
                        ~value=startValue,
                        ~refinedBy=[|
                          Lib_GraphQL_SelectorInput.make(
                            ~textPositionSelector=
                              Lib_GraphQL_SelectorInput.makeTextPositionSelectorInput(
                                ~start=startPosStart,
                                ~end_=startPosEnd,
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
                      Lib_GraphQL_SelectorInput.makeXPathSelectorInput(
                        ~value=endValue,
                        ~refinedBy=[|
                          Lib_GraphQL_SelectorInput.make(
                            ~textPositionSelector=
                              Lib_GraphQL_SelectorInput.makeTextPositionSelectorInput(
                                ~start=endPosStart,
                                ~end_=endPosEnd,
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
        (),
      ),
    (),
  );
};

let makeOnboardingAnnotations = (~identityId) => {
  let baseTs = Js.Date.make();

  [|
    makeOnboardingAnnotation(
      ~id=
        Lib_GraphQL.Annotation.makeIdFromComponent(
          ~identityId,
          ~annotationIdComponent=Uuid.makeV4(),
        ),
      ~textualTargetValue=
        "Welcome to Literal.\n\nLiteral is a textual annotation management system; a tool for bookmarking thoughts, ideas, and knowledge.\n\nAs you read, capture highlights of text with Literal. Literal automatically archives the source, context, and location of your annotation. Use tags to organize and add notes to annotations. Over time, revisit and leverage your annotations and their source context to augment the reading experience.\n\nSwipe left to learn more. ->",
      ~identityId,
      ~timestamp={
        let _ = baseTs->Js.Date.setMilliseconds(float_of_int(100));
        baseTs->Js.Date.toISOString;
      },
      (),
    ),
    makeOnboardingAnnotation(
      ~id=
        Lib_GraphQL.Annotation.makeIdFromComponent(
          ~identityId,
          ~annotationIdComponent=Uuid.makeV4(),
        ),
      ~textualTargetValue=
        "Capture annotations wherever you read.\n\nTo annotate web-based content, use the device sharesheet to share the URL with Literal, and use the source viewer to read and annotate.\n\nTo annotate other content types (e.g. PDF, Kindle), highlight the text within your reader application, take a screenshot, and use the device sharesheet to share the screenshot with Literal.\n\nSwipe left to learn more. ->",
      ~textualBodyValues=?None,
      ~identityId,
      ~timestamp={
        let _ = baseTs->Js.Date.setMilliseconds(float_of_int(99));
        baseTs->Js.Date.toISOString;
      },
      (),
    ),
    makeOnboardingAnnotation(
      ~id=
        Lib_GraphQL.Annotation.makeIdFromComponent(
          ~identityId,
          ~annotationIdComponent=Uuid.makeV4(),
        ),
      ~textualTargetValue=
        "Annotations are organized by tags and their source context. Tap the \"knowledge\" tag associated with this annotation and swipe left to explore example annotations. Tap an example annotation to see its source.\n\nIf you have any questions, feel to reach out to daniel@literal.io. Once you\'ve created annotations of your own, feel free to delete these example anotations.",
      ~textualBodyValues=[|"knowledge"|],
      ~identityId,
      ~timestamp={
        let _ = baseTs->Js.Date.setMilliseconds(float_of_int(98));
        baseTs->Js.Date.toISOString;
      },
      (),
    ),
    {
      let annotationId =
        Lib_GraphQL.Annotation.makeIdFromComponent(
          ~identityId,
          ~annotationIdComponent=Uuid.makeV4(),
        );
      makeOnboardingAnnotation(
        ~id=annotationId,
        ~textualBodyValues=[|"knowledge", "borges"|],
        ~textualTargetValue=
          "The Library exists ab aeterno. This truth, whose immediate corollary is the future eternity of the world, cannot be placed in doubt by any reasonable mind. Man, the imperfect librarian, may be the product of chance or of malevolent demiurgi; the universe, with its elegant endowment of shelves, of enigmatical volumes, of inexhaustible stairways for the traveler and latrines for the seated librarian, can only be the work of a god. To perceive the distance between the divine and the human, it is enough to compare these crude wavering symbols which my fallible hand scrawls on the cover of a book, with the organic letters inside: punctual, delicate, perfectly black, inimitably symmetrical.",
        ~specificTarget=
          makeSpecificTarget(
            ~annotationId,
            ~externalTargetId=
              "https://urbigenous.net/library/library_of_babel.html",
            ~externalTargetHashId=
              "56afdd38e2719fd3728c8f47df65761ebc306225d50eef13963ba32cc04b63ad",
            ~selectorStart=("/html/body/main/p[4]/text()[1]", 8, 27),
            ~selectorEnd=("/html/body/main/p[4]/text()[2]", 0, 663),
            ~cacheParsedUrl=
              "https://"
              ++ Constants.awsAmplifyConfig->AwsAmplify.Config.userFilesS3BucketGet
              ++ ".s3.amazonaws.com/shared-public-read/onboarding/library_of_babel/assets/https!urbigenous.net!library!library_of_babel.html",
          ),
        ~identityId,
        ~timestamp={
          let _ = baseTs->Js.Date.setMilliseconds(float_of_int(97));
          baseTs->Js.Date.toISOString;
        },
        (),
      );
    },
    {
      let annotationId =
        Lib_GraphQL.Annotation.makeIdFromComponent(
          ~identityId,
          ~annotationIdComponent=Uuid.makeV4(),
        );
      makeOnboardingAnnotation(
        ~id=annotationId,
        ~textualBodyValues=[|"knowledge", "clausewitz"|],
        ~textualTargetValue=
          "\"Many intelligence reports in war are contradictory; even more are false, and most are uncertain.... In short, most intelligence is false.\"",
        ~specificTarget=
          makeSpecificTarget(
            ~annotationId,
            ~externalTargetId=
              "https://en.m.wikipedia.org/wiki/Carl_von_Clausewitz",
            ~externalTargetHashId=
              "077739cb7f79212489f6da76e9443a94cfd85903aef3d86ab968ad290e17fedb",
            ~selectorStart=(
              "/html/body/div[1]/div/main/div[3]/div[1]/div/section[4]/p[7]/text()[1]",
              155,
              294,
            ),
            ~selectorEnd=(
              "/html/body/div[1]/div/main/div[3]/div[1]/div/section[4]/p[7]/text()[1]",
              0,
              294,
            ),
            ~cacheParsedUrl=
              "https://"
              ++ Constants.awsAmplifyConfig->AwsAmplify.Config.userFilesS3BucketGet
              ++ ".s3.amazonaws.com/shared-public-read/onboarding/clausewitz/assets/https!en.m.wikipedia.org!wiki!Carl_von_Clausewitz",
          ),
        ~identityId,
        ~timestamp={
          let _ = baseTs->Js.Date.setMilliseconds(float_of_int(97));
          baseTs->Js.Date.toISOString;
        },
        (),
      );
    },
    {
      let annotationId =
        Lib_GraphQL.Annotation.makeIdFromComponent(
          ~identityId,
          ~annotationIdComponent=Uuid.makeV4(),
        );
      makeOnboardingAnnotation(
        ~id=annotationId,
        ~textualBodyValues=[|"knowledge", "moby dick"|],
        ~textualTargetValue=
          "That inscrutable thing is chiefly what I hate; and be the white whale agent, or be the white whale principal, I will wreak that hate upon him. Talk not to me of blasphemy, man; I'd strike the sun if it insulted me. For could the sun do that, then could I do the other; since there is ever a sort of fair play herein, jealousy presiding over all creations. But not my master, man, is even that fair play. Who's over me? Truth hath no confines.",
        ~specificTarget=
          makeSpecificTarget(
            ~annotationId,
            ~externalTargetId=
              "https://www.gutenberg.org/files/2701/2701-h/2701-h.htm#link2HCH0036",
            ~externalTargetHashId=
              "f68db93ffb29202c7488720b13154b8c4625a6d2c0b43357c9d44bcde1ac67d6",
            ~selectorStart=("/html/body/p[695]/text()[1]", 662, 2379),
            ~selectorEnd=("/html/body/p[695]/text()[1]", 0, 1140),
            ~cacheParsedUrl=
              "https://"
              ++ Constants.awsAmplifyConfig->AwsAmplify.Config.userFilesS3BucketGet
              ++ ".s3.amazonaws.com/shared-public-read/onboarding/moby_dick/assets/https!www.gutenberg.org!files!2701!2701-h!2701-h.htm%23link2HCH0036",
          ),
        ~identityId,
        ~timestamp={
          let _ = baseTs->Js.Date.setMilliseconds(float_of_int(97));
          baseTs->Js.Date.toISOString;
        },
        (),
      );
    },
  |]
  ->Belt.Array.reverse;
};
