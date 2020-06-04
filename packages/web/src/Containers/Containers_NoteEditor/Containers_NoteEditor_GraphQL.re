module GetHighlightFragment = [%graphql
  {|
    fragment editorHighlightFragment on Highlight {
      id
      text
      tags {
        items {
          tag {
            text
          }
        }
      }
    }
  |}
];

module UpdateHighlightMutation = [%graphql
  {|
  mutation UpdateHighlight($input: UpdateHighlightInput!) {
    updateHighlight(input: $input) {
      id
      text
    }
  }
|}
];
