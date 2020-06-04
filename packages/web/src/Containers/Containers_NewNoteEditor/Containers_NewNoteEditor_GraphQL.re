module CreateHighlightMutation = [%graphql
  {|
    mutation CreateHighlight(
      $createHighlightInput: CreateHighlightInput!
      $createTagsInput: [CreateTagInput!]!
    ) {
      createHighlight(input: $createHighlightInput) {
        id
        createdAt
      }
      createTags(input: $createTagsInput) {
        id
        createdAt
      }
    }
  |}
];
