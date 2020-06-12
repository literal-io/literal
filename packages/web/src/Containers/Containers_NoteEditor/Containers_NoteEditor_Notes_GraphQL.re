module GetHighlightFragment = [%graphql
  {|
    fragment editorHighlightFragment on Highlight {
      id
      text
      tags {
        items {
          id
          createdAt
          tag {
            id
            text
          }
        }
      }
    }
  |}
];

module UpdateHighlightMutation = [%graphql
  {|
  mutation UpdateHighlight(
    $updateHighlightInput: UpdateHighlightInput!
    $createTagsInput: [CreateTagInput!]!
    $createHighlightTagsInput: [CreateHighlightTagInput!]!
    $deleteHighlightTagsInput: [DeleteHighlightTagInput!]!
  ) {
    updateHighlight(input: $updateHighlightInput) {
      id
      text
    }
    createTags(input: $createTagsInput) {
      id
      createdAt
    }
    createHighlightTags(input: $createHighlightTagsInput) {
      id
      createdAt
    }
    deleteHighlightTags(input: $deleteHighlightTagsInput) {
      id 
    }
  }
|}
];
