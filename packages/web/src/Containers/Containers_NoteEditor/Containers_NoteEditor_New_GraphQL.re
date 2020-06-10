module CreateHighlightMutation = [%graphql
  {|
    mutation CreateHighlight(
      $createHighlightInput: CreateHighlightInput!
      $createTagsInput: [CreateTagInput!]!
      $createHighlightTagsInput: [CreateHighlightTagInput!]!
    ) {
      createHighlight(input: $createHighlightInput) {
        id
        createdAt
      }
      createTags(input: $createTagsInput) {
        id
        createdAt
      }
      createHighlightTags(input: $createHighlightTagsInput) {
        id
      }
    }
  |}
];
