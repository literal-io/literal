module GetProfileFragment = [%graphql
  {|
    fragment onboardingProfileFragment on Profile {
      id
      owner
    }
  |}
];

module OnboardingMutation = [%graphql
  {|
    mutation onboarding(
      $updateProfileInput: UpdateProfileInput!
      $createHighlightInput1: CreateHighlightInput!
      $createHighlightInput2: CreateHighlightInput!
      $createHighlightInput3: CreateHighlightInput!
    ) {
      updateProfile(input: $updateProfileInput) {
        id
        isOnboarded
      }
      createHighlight1: createHighlight(input: $createHighlightInput1) {
        id
        createdAt
        __typename
        ...Containers_NoteEditor_Notes_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
        ...Containers_NoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
      }
      createHighlight2: createHighlight(input: $createHighlightInput2) {
        id
        createdAt
        __typename
        ...Containers_NoteEditor_Notes_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
        ...Containers_NoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
      }
      createHighlight3: createHighlight(input: $createHighlightInput3) {
        id
        createdAt
        __typename
        ...Containers_NoteEditor_Notes_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
        ...Containers_NoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
      }
    }
  |}
];
