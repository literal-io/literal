module CreateHighlightMutation = [%graphql
  {|
    mutation CreateHighlight($input: CreateHighlightInput!) {
      createHighlight(input: $input) {
        id
        createdAt
      }
    }
  |}
];
