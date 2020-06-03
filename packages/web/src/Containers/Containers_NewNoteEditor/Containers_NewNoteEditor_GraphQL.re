module CreateHighlightMutation = [%graphql
  {|
    mutation CreateHighlight($createHighlightInput: CreateHighlightInput!) {
      createHighlight(input: $createHighlightInput) {
        id
        createdAt
      }
    }
  |}
];
