module UpdateHighlightMutation = [%graphql
  {|
    mutation UpdateHighlight(
      $input: UpdateHighlightAndTagsInput!
    ) {
      updateHighlightAndTags(input: $input) {
        updateHighlight {
          id
          text
        }
        createTags {
          id
          createdAt
        }
        createHighlightTags {
          id
          createdAt
        }
        createHighlightTags {
          id
          createdAt
        }
      }
    }
  |}
];

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
