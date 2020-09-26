module GetAnnotationFragment = [%graphql
  {|
    fragment headerAnnotationFragment on Annotation {
      id
      body {
        ... on TextualBody {
          __typename
          id
          value
          purpose
        }
      }
    }
  |}
];

module DeleteAnnotationMutation = [%graphql
  {|
    mutation deleteAnnotation($input: DeleteAnnotationInput!) {
      deleteAnnotation(input: $input) {
        annotation {
          id
        }
      }
    }
  |}
];

