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
