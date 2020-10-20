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
      $createAnnotationInput1: CreateAnnotationInput!
      $createAnnotationInput2: CreateAnnotationInput!
      $createAnnotationInput3: CreateAnnotationInput!
    ) {
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
    }
  |}
];
