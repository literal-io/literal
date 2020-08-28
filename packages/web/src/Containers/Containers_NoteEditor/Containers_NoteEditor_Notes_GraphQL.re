module GetAnnotationFragment = [%graphql
  {|
    fragment editorAnnotationFragment on Annotation {
      id
      body {
        ... on TextualBody {
          id
          value
          purpose
        }
      }
      target {
        ... on TextualTarget {
          value
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
