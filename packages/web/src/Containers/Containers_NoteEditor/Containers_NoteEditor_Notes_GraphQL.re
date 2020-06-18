module GetTagFragment = [%graphql
  {|
    fragment GetTag on Tag {
      id
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

module UpdateHighlightMutation = [%graphql
  {|
  mutation UpdateHighlightAndTags($input: UpdateHighlightAndTagsInput!) {
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
      deleteHighlightTags {
        id
      }
    }
  }
|}
];
