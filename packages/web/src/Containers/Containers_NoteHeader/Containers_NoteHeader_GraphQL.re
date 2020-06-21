module GetHighlightFragment = [%graphql
  {|
    fragment headerHighlightFragment on Highlight {
      id
      tags {
        items {
          id
        }
      }
    }
  |}
];

module DeleteHighlightMutation = [%graphql
  {|
    mutation deleteHighlight(
      $deleteHighlightInput: DeleteHighlightInput!
      $deleteHighlightTagsInput: [DeleteHighlightTagInput!]!
    ) {
      deleteHighlight(input: $deleteHighlightInput) {
        id
      }
      deleteHighlightTags(input: $deleteHighlightTagsInput) {
        id
      }
    }
  |}
];
