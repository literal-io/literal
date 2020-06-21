module CreateHighlightMutation = [%graphql
  {|
    mutation CreateHighlight($input: CreateHighlightAndTagsInput!) {
      createHighlightAndTags(input: $input) {
        createHighlight {
          id
          createdAt
        }
        createTags {
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
