module GetHighlightFragment = [%graphql
  {|
    fragment headerHighlightFragment on Highlight {
      id
    }
  |}
];

module DeleteHighlightMutation = [%graphql
  {|
    mutation deleteHighlight($input: DeleteHighlightInput!) {
      deleteHighlight(input: $input) {
        id
      }
    }
  |}
];

